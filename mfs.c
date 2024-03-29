/*
   Name: Carolyn Nguyen
   ID: 1001549627

   Name: Bridget Gregory
   ID: 1001658617

*/

// The MIT License (MIT)
//
// Copyright (c) 2020 Trevor Bakker
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <stdint.h>

//changed it to 4 to fit read <filename> <position> <num of bytes> function
#define MAX_NUM_ARGUMENTS 4

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
// so we need to define what delimits our tokens.
// In this case  white space
// will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size


// FAT32 variables
char BS_OEMName[8];
int16_t BPB_BytesPerSec;
int8_t BPB_SecPerClus;
int16_t BPB_RsvdSecCnt;
int8_t BPB_NumFATs;
int16_t BPB_RootEntCnt;
char BS_VolLab[11];
int32_t BPB_FATSz32;
int32_t BPB_RootClus;


// given in class - FAT-1.pdf
struct DirectoryEntry
{
   char DIR_Name[11];
   uint8_t DIR_Attr;
   uint8_t Unused1[8];
   uint16_t DIR_FirstClusterHigh;
   uint8_t Unused2[4];
   uint16_t DIR_FirstClusterLow;
   uint32_t DIR_FileSize;
};
struct DirectoryEntry dir[16];


// global variables
int fileOpen; //if 1, file is open; if 0, file is closed
FILE *fp; //file pointer to fat32 image
uint8_t attrHolder; //placeholder for attributes
char fnHolder[11]; //placeholder for file name
char name[12]; //reserve 12 characters for file name (11 bits plus 1 null)


// compare converts the given filename to an expanded filename,
//   goes through DirectoryEntry and finds if a DIR_Name that equals filename,
//   returns 0 if match, 1 if no match
// - from compare.c function from class GitHub
int compare(char *input, char *nameInDir)
{
   char IMG_Name[11];

   char in[11];
   char expanded_name[12];

   strncpy(IMG_Name, nameInDir, 11);
   strncpy(in, input, 11);

   if(strncmp(input, "..", 2) != 0)
   {
      memset(expanded_name, ' ', 12);
      char *token = strtok(in, ".");
      strncpy(expanded_name, token, strlen(token));
      token = strtok(NULL, ".");

      if(token)
      {
         strncpy((char *)(expanded_name + 8), token, strlen(token));
      }

      expanded_name[11] = '\0';

      int i;
      for (i = 0; i < 11; i++)
      {
         expanded_name[i] = toupper(expanded_name[i]);
      }
   }
   else
   {
      strncpy(expanded_name, "..", 2);
      expanded_name[3] = '\0';
      if(strncmp(expanded_name, IMG_Name, 2) == 0)
      {
         return 0;
      }
      return 1;
   }

   if(strncmp(expanded_name, IMG_Name, 11) == 0)
   {
      return 0;
   }
   return 1;
}


/* (From FAT-1.pdf on Canvas)
Function: LBAtoOffset
Parameters: The current sector number that points to a block of data
Returns: The value of the address for that block of data
Description: Finds the starting address of a block of data given the sector number
corresponding to that data block
*/
int LBAtoOffset(int32_t sector)
{
   return ((sector - 2) * BPB_BytesPerSec) + (BPB_BytesPerSec * BPB_RsvdSecCnt)
   + (BPB_NumFATs * BPB_FATSz32 * BPB_BytesPerSec);
}


/* (from FAT-1.pdf on Canvas)
Name: NextLB
Purpose: Given a logical block address, look up into the first FAT and return the
logical block address of the block in the file. If there is no further blocks
then return -1
*/
int16_t NextLB(uint32_t sector)
{
   uint32_t FATAddress = (BPB_BytesPerSec * BPB_RsvdSecCnt) + (sector * 4);
   int16_t val;
   fseek(fp, FATAddress, SEEK_SET);
   fread(&val, 2, 1, fp);

   return val;
}


/*
This command shall open a fat32 image.  Filenames of fat32 images
shall not contain spaces and shall be limited to 100 characters.
If the file is not found your program shall output:
“Error: File system image not found.”.
If a file system is already opened then your program shall output:
“Error: File system image already open.”.
*/
void openFile(char* fn) //5 points
{
   //open file for fn: filename in read mode
   fp = fopen(fn, "r");

   //check if the file is already open
   if(fileOpen == 1)
   {
      printf("Error: File system image already open.\n");
      return;
   }

   else
   {
      if(fp != NULL)
      {
         fileOpen=1;

         // - reading in BPB variables
         //   get BytesPerSec which starts at 11, size of 2
         fseek(fp, 11, SEEK_SET);
         fread(&BPB_BytesPerSec, 2, 1, fp);

         //   get BPB_SecPerClus which starts at 13, size of 1
         fseek(fp, 13, SEEK_SET);
         fread(&BPB_SecPerClus, 1, 1, fp);

         //   get BPB_RsvdSecCnt which starts at 14, size of 2
         fseek(fp, 14, SEEK_SET);
         fread(&BPB_RsvdSecCnt, 2, 1, fp);

         //   get BPB_NumFATS which starts at 16, size of 1
         fseek(fp, 16, SEEK_SET);
         fread(&BPB_NumFATs, 1, 1, fp);

         //   get BPB_FATSz32 which starts at 36, size of 4
         fseek(fp, 36, SEEK_SET);
         fread(&BPB_FATSz32, 4, 1, fp);

         // - file pointer placed at root
         fseek(fp, BPB_NumFATs * (BPB_FATSz32 * BPB_BytesPerSec) +
         (BPB_RsvdSecCnt * BPB_BytesPerSec), SEEK_SET);
         fread(&dir[0],sizeof(struct DirectoryEntry), 16, fp);
      }

      //let user know if file could not open
      else
      {
         printf("Error: File system image not found.\n");
      }
   }
}



/*
This command shall close the fat32 image.  If the file system
is not currently open your program shall output:
“Error: File system not open.”
Any command issued after a close, except for
open, shall result in “Error: File system image must be opened first.”
*/
void closeFile() //5 points
{
   if(fileOpen)
   {
      fclose(fp);
      // change condition of fileOpen to closed
      fileOpen=0;
   }

   else
   {
      printf("Error: File system image must be opened first.\n");
   }
}



/*
This command shall print out
information about the file system in
both hexadecimal and base 10:
-BPB_BytesPerSec
-BPB_SecPerClus
-BPB_RsvdSecCnt
-BPB_NumFATS
-BPB_FATSz32
*/
void fileInfo() //10 points
{
   //if open, print info
   if(fileOpen)
   {
      // - print out FAT32 variables
      printf("BPB_BytesPerSec: \t%d \t%x\n", BPB_BytesPerSec, BPB_BytesPerSec);
      printf("BPB_SecPerClus: \t%d \t%x\n", BPB_SecPerClus, BPB_SecPerClus);
      printf("BPB_RsvdSecCnt: \t%d \t%x\n", BPB_RsvdSecCnt, BPB_RsvdSecCnt);
      printf("BPB_NumFATS: \t\t%d \t%x\n", BPB_NumFATs, BPB_NumFATs);
      printf("BPB_FATSz32: \t\t%d \t%x\n", BPB_FATSz32, BPB_FATSz32);
   }

   //else print that no file is open
   else
   {
      printf("Error: File system image must be opened first.\n");
   }
}


/*
This command shall print the attributes and
starting cluster number of the file or directory name.
If the parameter is a directory name then the size shall be 0.
If the file or directory does not exist
then your program shall output “Error: File not found”.
*/
void fileStat(char *fn) //10 points
{
   //if open, print stat
   if(fileOpen)
   {
      // check if filename was given
      if(fn == 0)
      {
         printf("Error: Missing parameter.\n");
         return;
      }

      // - look up directory and compare filename for match
      // - find index in dir[] where filename matches
      int index=-1;
      for(int i=0; i<16; i++)
      {
         if(compare(fn, dir[i].DIR_Name)==0)
         {
            index = i;
         }
      }

      //if no match was found
      if(index==-1)
      {
         printf("Error: File not found.\n");
         return;
      }

      // - print out attributes and starting cluster
      printf("File Size: \t\t%d\n", dir[index].DIR_FileSize);
      printf("First Cluster Low: \t%d\n", dir[index].DIR_FirstClusterLow);
      printf("DIR_ATTR: \t\t%d\n", dir[index].DIR_Attr);
      printf("First Cluster High: \t%d\n", dir[index].DIR_FirstClusterHigh);
   }

   //else print that no file is open
   else
   {
      printf("Error: File system image must be opened first.\n");
   }
}


/*
This command shall retrieve the file from the FAT 32
image and place it in your current working directory.
If the file or directory does not exist then your program
shall output “Error: File not found”.
*/
void getFile(char *fn) //15 points
{
   //if open, get file
   if(fileOpen)
   {
      //check if filename was given
      if(fn == 0)
      {
         printf("Error: Missing parameter.\n");
         return;
      }

      // - find index in dir[] where filename matches
      int index=-1;
      for(int i=0; i<16; i++)
      {
         if(compare(fn, dir[i].DIR_Name)==0)
         {
            index = i;
         }
      }

      //if no match was found
      if(index==-1)
      {
         printf("Error: File not found.\n");
         return;
      }

      // - reading in BPB variables
      fseek(fp, 11, SEEK_SET);
      fread(&BPB_BytesPerSec, 2, 1, fp);
      fseek(fp, 13, SEEK_SET);
      fread(&BPB_SecPerClus, 1, 1, fp);
      fseek(fp, 14, SEEK_SET);
      fread(&BPB_RsvdSecCnt, 2, 1, fp);
      fseek(fp, 16, SEEK_SET);
      fread(&BPB_NumFATs, 1, 1, fp);
      fseek(fp, 36, SEEK_SET);
      fread(&BPB_FATSz32, 4, 1, fp);

      // - get low cluster number
      int cluster = dir[index].DIR_FirstClusterLow;
      // - save file size
      uint32_t fsize = dir[index].DIR_FileSize;
      // - calculate offset
      int offset = LBAtoOffset(cluster);
      // - fseek to that offset
      fseek(fp, offset, SEEK_SET); //fp is pointing exactly where the file begins
      // - get file pointer to output file
      FILE *outputfp = fopen(fn, "w");
      // - create buffer with size 512 because 512 = BPB_BytesPerSec
      uint8_t buffer[512];

      // - for all clusters in the file
      //    - read from disk image, into buffer
      //    - write from buffer, to new output file
      while(fsize >= BPB_BytesPerSec)
      {
         fread(buffer, 512, 1, fp);
         fwrite(buffer, 512, 1, outputfp);

         // - update file size
         fsize = fsize - BPB_BytesPerSec;
         // - update offset
         if(cluster > -1)
         {
            cluster = NextLB(cluster);
            offset = LBAtoOffset(cluster);
            fseek(fp, offset, SEEK_SET);
         }

      }

      //need to handle last block (where file size < 512 due to external fragmentation)
      if(fsize > 0)
      {
         fread(buffer, fsize, 1, fp);
         fwrite(buffer, fsize, 1, outputfp);
      }

      //close connection to output file
      fclose(outputfp);
   }

   //else print that no file is open
   else
   {
      printf("Error: File system image must be opened first.\n");
   }
}


/*
This command shall change the current working directory
to the given directory. Your program shall support relative
paths, e.g cd ../name and absolute paths. (HE SAID ABSOLUTE PATH
DOESN'T NEED TO BE DONE IN CLASS)
*/
void changeDir(char *fn) //10 points
{
   if(fileOpen)
   {
      int i;
      for(i = 0; i < 16; i++)
      {
         if(!compare(fn, dir[i].DIR_Name))
         {
            int cluster = dir[i].DIR_FirstClusterLow;

            if(cluster == 0)
            {
               cluster = 2;
            }
            int offset = LBAtoOffset(cluster);

            fseek(fp, offset, SEEK_SET);
            fread(&dir[0], sizeof(struct DirectoryEntry), 16, fp);

            break;
         }
      }

      char *direct;
      direct = strtok(fn, "/");
      for(i = 0; i < 16; i++)
      {
         if(!compare(fn, dir[i].DIR_Name))
         {
            int cluster = dir[i].DIR_FirstClusterLow;
            int offset = LBAtoOffset(cluster);

            //checks for root
            if(cluster == 0)
            {
               cluster = 2;
            }

            fseek(fp, offset, SEEK_SET);
            fread(&dir[0], sizeof(struct DirectoryEntry), 16, fp);

            break;
         }
      }

      while((direct = strtok(NULL, "/")))
      {
         int cluster = dir[i].DIR_FirstClusterHigh;

         //checks for root
         if(cluster == 0)
         {
            cluster = 2;
         }
         int offset = LBAtoOffset(cluster);
         fseek(fp, offset, SEEK_SET);
         fread(&dir[0], sizeof(struct DirectoryEntry), 16, fp);
      }
   }
   else
   {
      printf("Error: File system image must be opened first.\n");
   }
}


/*
Lists the directory contents.
Your program shall support listing “.” and “..” .
Your program shall not list deleted files or system
volume names.
*/
void listDir() //10 points
{
   //if open, list directory
   if(fileOpen)
   {
      // - go through struct DirectoryEntry and print out valid filenames/directory names
      for(int i=0; i<16; i++)
      {
         // - check if file is read only (0x01), subdirectory (0x10), or archive flag (0x20)
         // - check that its also not a deleted file (0xe5)
         if((dir[i].DIR_Attr == 0x01 || dir[i].DIR_Attr == 0x10 || dir[i].DIR_Attr == 0x20)
         && dir[i].DIR_Name[0] != '?')
         {
            //reserve 12 characters for file name (11 bits plus 1 null)
            char name[12];
            memcpy(name, dir[i].DIR_Name, 11);
            name[11]='\0';
            printf("%s\n", name);
         }
      }
   }

   //else print error
   else
   {
      printf("Error: File system image must be opened first.\n");
   }
}


/*
Reads from the given file at the position, in bytes,
specified by the position parameter and output
the number of bytes specified.
*/
void readFile(char *fn, char *pos, char *n_b) //10 points
{
   //if open, read file
   if(fileOpen)
   {
      //check if filename was given
      if(fn == 0 || pos == NULL || n_b == NULL)
      {
         printf("Error: Missing parameter.\n");
         return;
      }

      // - get index of dir[] where filename matches
      int index=-1;
      for(int i=0; i<16; i++)
      {
         if(compare(fn, dir[i].DIR_Name)==0)
         {
            index = i;
         }
      }

      //if no match was found
      if(index==-1)
      {
         printf("Error: File not found.\n");
         return;
      }

      // - change position and n_bytes parameters to ints
      int position=atoi(pos);
      int n_bytes=atoi(n_b);
      // - get low cluster of specific filename
      int cluster = dir[index].DIR_FirstClusterLow;
      // - calculate offset
      int offset = LBAtoOffset(cluster);
      // - fseek to that offset
      fseek(fp, offset, SEEK_SET); //fp is pointing exactly where the file begins
      // - create buffer with size 512 because 512 = BPB_BytesPerSec
      uint8_t buffer[512];

      // - fseek to the specified position depending on current position of fp
      fseek(fp, position, SEEK_CUR);
      // - read info into buffer for number of specified bytes
      fread(buffer, n_bytes, 1, fp);

      // - print out values in buffer for specified number of bytes
      for(int i=0; i<n_bytes; i++)
      {
         printf("%d ", buffer[i]);
      }
      printf("\n"); // print new line to match sample output

   }

   //print error
   else
   {
      printf("Error: File system image must be opened first.\n");
   }
}


/*
Deletes the file from the file system
*/
void deleteFile(char *fn) //10 points
{
   //if open, delete file in directory
   if(fileOpen)
   {
      // - get index of dir[] where filename matches
      int index=-1;
      for(int i=0; i<16; i++)
      {
         if(compare(fn, dir[i].DIR_Name)==0)
         {
            index = i;
         }
      }

      //if no match was found
      if(index==-1)
      {
         printf("Error: File not found.\n");
         return;
      }

      else
      {
         //save attributes into a placeholder
         attrHolder = dir[index].DIR_Attr;
         //delete file by setting attribute to 0xe5
         dir[index].DIR_Attr = 0xe5;
         //save file name into a placeholder
         strncpy(fnHolder, dir[index].DIR_Name, 11);
         //change file name to show that it has been deleted
         strcpy(dir[index].DIR_Name, "?");
      }
   }

   //else print error
   else
   {
      printf("Error: File system image must be opened first.\n");
   }
}


/*
Un-deletes the file from the file system
*/
void restoreFile(char *fn) //10 points
{
   //if 0, not a deleted file; if 1, deleted file
   int deleted = 0;
   int i;

   if(fileOpen)
   {
      for(i = 0; i < 16; i++)
      {
         //if there is no deleted files, break out of loop
         if(!strcmp(dir[i].DIR_Name,"?"))
         {
            deleted = 1;
            break;
         }
      }
      //if it is a deleted file
      if(deleted)
      {
         //assign name placeholder to DIR_Name
         strncpy(dir[i].DIR_Name, fnHolder, 11);
         //assign attributes placeholder to DIR_Attr
         dir[i].DIR_Attr = attrHolder;
      }
      else
      {
         printf("Error: File not found. \n");
      }
   }

   //else print error
   else
   {
      printf("Error: File system image must be opened first.\n");
   }
}


int main()
{

   char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );


   while( 1 )
   {
      // -------------- GIVEN FROM MFS.C ON GITHUB ------------------------------
      // Print out the mfs prompt
      printf ("mfs> ");

      // Read the command from the commandline.  The
      // maximum command that will be read is MAX_COMMAND_SIZE
      // This while command will wait here until the user
      // inputs something since fgets returns NULL when there
      // is no input
      while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );

      /* Parse input */
      char *token[MAX_NUM_ARGUMENTS];

      int   token_count = 0;

      // Pointer to point to the token
      // parsed by strsep
      char *arg_ptr;

      char *working_str  = strdup( cmd_str );

      // we are going to move the working_str pointer so
      // keep track of its original value so we can deallocate
      // the correct amount at the end
      char *working_root = working_str;

      // Tokenize the input stringswith whitespace used as the delimiter
      while ( ( (arg_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) &&
      (token_count<MAX_NUM_ARGUMENTS))
      {
         token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE );
         if( strlen( token[token_count] ) == 0 )
         {
            token[token_count] = NULL;
         }
         token_count++;
      }



      // ------------------------------ COMMANDS ------------------------------

      if(token[0] == NULL)
      {
         continue;
      }

      //exit(1) if user wants to exit/quit
      if(strcmp(token[0], "exit") == 0 || strcmp(token[0], "quit") == 0 )
      {
         exit(1);
         return 0;
      }

      if(strcmp(token[0], "open") == 0)
      {
         // pass in filename parameter (token[1])
         openFile(token[1]);
         continue;
      }

      if(strcmp(token[0], "close") == 0)
      {
         closeFile();
      }

      if(strcmp(token[0], "info") == 0)
      {
         fileInfo();
      }

      if(strcmp(token[0], "stat")==0)
      {
         // pass in filename parameter (token[1])
         fileStat(token[1]);
      }

      if(strcmp(token[0], "get") == 0)
      {
         // pass in filename parameter (token[1])
         getFile(token[1]);
      }

      if(strcmp(token[0], "cd") == 0)
      {
         // pass in filename parameter (token[1])
         changeDir(token[1]);
      }

      if(strcmp(token[0], "ls") == 0)
      {
         listDir();
      }

      if(strcmp(token[0], "read") == 0)
      {
         // pass in filename parameter (token[1]), position (token[2]), number of bytes (token[3])
         readFile(token[1], token[2], token[3]);
      }

      if(strcmp(token[0], "del") == 0)
      {
         // pass in filename parameter (token[1])
         deleteFile(token[1]);
      }

      if(strcmp(token[0], "undel") == 0)
      {
         // pass in filename parameter (token[1])
         restoreFile(token[1]);
      }



      free( working_root );

   }
   return 0;
}
