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

#define MAX_NUM_ARGUMENTS 3

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

// given in class
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

//global variables
int fileOpen; //if 1, file is open; if 0, file is closed
FILE *fp; //file pointer to fat32 image


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

   // //file is not already opened and can be opened successfully
   // if(fp != NULL)
   // {
   //    fileOpen=1;
   //    printf("File opened successfully\n");
   // }
   //
   // //check if the file is already open
   // else if(fileOpen == 1)
   // {
   //    printf("Error: File system image already open.\n");
   //    return;
   // }
   //
   // else
   // {
   //    printf("Error: File system image not found.\n");
   // }

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
         printf("File opened successfully\n");
      }
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
      printf("Closing file...\n");
      fclose(fp);
      fileOpen=0;
   }
   else
   {
      printf("Error: File system not open.");
   }
}
//still need to add "Error: File system image must be opened first" part


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
   int16_t BPB_BytesPerSec;
   int16_t BPB_SecPerClus;
   int16_t BPB_RsvdSecCnt;
   int16_t BPB_NumFATS;
   int16_t BPB_FATSz32;
   //if open, print info
   //else print that no file is open
   if(fileOpen)
   {
    //code
    //get BytesPerSec which starts at 11, size of 2
    fseek(fp, 11, SEEK_SET);
    fread(&BPB_BytesPerSec, 2, 1, fp);
    printf("BPB_BytesPerSec: %d\n", BPB_BytesPerSec);

    //get BPB_SecPerClus which starts at 13, size of 1
    fseek(fp, 13, SEEK_SET);
    fread(&BPB_SecPerClus, 1, 1, fp);
    printf("BPB_SecPerClus: %d\n", BPB_SecPerClus);

    //get BPB_RsvdSecCnt which starts at 14, size of 2
    fseek(fp, 14, SEEK_SET);
    fread(&BPB_RsvdSecCnt, 2, 1, fp);
    printf("BPB_RsvdSecCnt: %d\n", BPB_RsvdSecCnt);

    //get BPB_NumFATS which starts at 16, size of 1
    fseek(fp, 16, SEEK_SET);
    fread(&BPB_NumFATS, 1, 1, fp);
    printf("BPB_NumFATS: %d\n", BPB_NumFATS);

    //get BPB_FATSz32 which starts at 22, size of 2
    fseek(fp, 22, SEEK_SET);
    fread(&BPB_FATSz32, 2, 1, fp);
    printf("BPB_FATSz32: %d\n", BPB_FATSz32);
    // ^^^ check to make sure thats the right size
   }
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
void fileStat() //10 points
{
   //if open, print info
   //else print that no file is open
   if(fileOpen)
   {
    //code
   }
   else
   {
      printf("Error: File system image must be opened first.");
   }
}


/*
This command shall retrieve the file from the FAT 32
image and place it in your current working directory.
If the file or directory does not exist then your program
shall output “Error: File not found”.
*/
void getFile() //15 points
{
   //if open, print info
   //else print that no file is open
   if(fileOpen)
   {
    //code
   }
   else
   {
      printf("Error: File system image must be opened first.");
   }
}

/*
This command shall change the current working directory
to the given directory. Your program shall support relative
paths, e.g cd ../name and absolute paths. (HE SAID ABSOLUTE PATH
DOESN'T NEED TO BE DONE IN CLASS)
*/
void changeDir() //10 points
{
   //if open, print info
   //else print that no file is open
   if(fileOpen)
   {
    //code
   }
   else
   {
      printf("Error: File system image must be opened first.");
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
   //if open, print info
   //else print that no file is open
   if(fileOpen)
   {
    //code
   }
   else
   {
      printf("Error: File system image must be opened first.");
   }
}


/*
Reads from the given file at the position, in bytes,
specified by the position parameter and output
the number of bytes specified.
*/
void readFile() //10 points
{
   //if open, print info
   //else print that no file is open
   if(fileOpen)
   {
    //code
   }
   else
   {
      printf("Error: File system image must be opened first.");
   }
}


/*
Deletes the file from the file system
*/
void deleteFile() //10 points
{
   //if open, print info
   //else print that no file is open
   if(fileOpen)
   {
    //code
   }
   else
   {
      printf("Error: File system image must be opened first.");
   }
}


/*
Un-deletes the file from the file system
*/
void restoreFile() //10 points
{

}


int main()
{

   char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );

   //int fileOpen = 0; //if 1, file is open; if 0, file is closed

   while( 1 )
   {
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

      // ----------------------------------------------------------------------
      // Now print the tokenized input as a debug check
      // \TODO Remove this code and replace with your FAT32 functionality

      int token_index  = 0;
      for( token_index = 0; token_index < token_count; token_index ++ )
      {
         printf("token[%d] = %s\n", token_index, token[token_index] );
      }
      // ----------------------------------------------------------------------


      // ------------------------------ COMMANDS ------------------------------

      if(token[0] == NULL)
      {
         // printf("NULL token[0]\n");
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
         printf("Calling openFile function...\n");
         //pass in filename (token[1]/argv[1]) to openFile function
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

      if(strcmp(token[0], "stat"))
      {
       //fileStat();
      }

      if(strcmp(token[0], "get") == 0)
      {
       //getFile();
      }

      if(strcmp(token[0], "cd") == 0)
      {
       //changeDir();
      }

      if(strcmp(token[0], "ls") == 0)
      {
       //listDir();
      }

      if(strcmp(token[0], "read") == 0)
      {
       //readFile();
      }

      if(strcmp(token[0], "del") == 0)
      {
       //deleteFile();
      }

      if(strcmp(token[0], "undel") == 0)
      {
       //restoreFile();
      }



      free( working_root );

   }
   return 0;
}
