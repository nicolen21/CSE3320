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
  // opening file
  file = fopen(fn, 'r');

  if(file != NULL)
  {
    fileOpen = 1; //set to 1 to indicate that the file is opened

    //insert file reading here

  }
  else if(fileOpen = 1)
  {
    printf("Error: File system image already open.\n");
  }
  else
  {
    printf("Error: File system image not found.\n");
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
    fclose(file);
    fileOpen = 0;
  }
  else
  {
    printf("Error: File system not open.");
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
  This command shall print the attributes and 
  starting cluster number of the file or directory name.  
  If the parameter is a directory name then the size shall be 0. 
  If the file or directory does not exist 
  then your program shall output “Error: File not found”.
*/
void fileStat() //10 points
{
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
  if(fileOpen)
  {
    /* FOR RELATIVE
      char * directory;
      directory = strtok(token[i],"/");

      [insert cd without relative code here]

      while(directory = strtok(NULL,"/"))
      {
        lookup for cluster (IF CLUSTER == 0, SET CLUSTER = 2)
        convert offset
        read dir
        (DONT NEED TO BREAK)
      }
    */

    /* HELPFUL CODE
      int last_offset = 0x1001100;
      offset = last_offset;
    */

    /*PSEUDOCODE-ISH THAT WAS WRITTEN IN CLASS FOR CD
      for(i = 0; i < 16; i++)
      {
        if(compare(token[i], dir[i].DIRxxx)) //couldn't read where the xxx's are
        {
          int cluster = dir[i].clusterLow;
          int offset = LBktoOffset(cluster);
          fseek(fp,offset,SEEKSET);
          fseek(dir[0],sizeof(DP),11,fp);
        }
        break;
      }
    */
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
  //make sure to set fileOpen = 0 after deletion
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

  int fileOpen = 0; //if 1, file is open; if 0, file is closed

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

    // --------------------------- COMMANDS ---------------------------


    if(token[0] == NULL)
    {
      continue;
    }

    if(strcmp(token[0], "exit") == 0 || strcmp(token[0], "quit") == 0 )
    {
      return 0;
    }

    if(strcmp(token[0], "open") == 0)
    {
      fileOpen(args[0]);
      continue;
    }

    if(strcmp(token[0], "close") == 0)
    {
      closeFile();
    }

    if(strcmp(token[0], "info") == 0)
    {
      //fileInfo();
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

  }
  return 0;
}
