#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
char buf[100];
char *dir;
char *srcDir;
char *destDir;
int main(int argc, char **argv)
{
	printf("\nWellcome to my mini SHELL developed as the OS course assignment\n");
	while (1)
	{
		printf("\nMahsa_Shell: ");
		char in_char;
		int num_char = 0;
		char *input_Command = (char *)malloc(100);

		if (input_Command == 0)
		{
			fprintf(stderr, "Memory allocation failed\n");
			exit(EXIT_FAILURE);
		}
		for (int i = 0; i < 100; i++)
		{
			in_char = getc(stdin);
			if (in_char == '\n')
			{
				input_Command[num_char] = '\0';
				break;
			}
			else
			{
				input_Command[num_char] = in_char;
				num_char++;
			}
		}
		//printf(input_Command);
		char **input_Command_tokenized = (char **)malloc(1024);
		char *input_Command_spaced;
		int i = 0;
		int into_place = 0; //index of the command "into"
		if (input_Command_tokenized == 0)
		{
			fprintf(stderr, "Memory allocation failed\n");
			exit(EXIT_FAILURE);
		}
		input_Command_spaced = strtok(input_Command, " "); // words of the input command line is separated by space character
		while (input_Command_spaced != NULL)
		{
			input_Command_tokenized[i] = input_Command_spaced;
			i++;
			input_Command_spaced = strtok(NULL, " ");
		}
		input_Command_tokenized[i] = NULL;

		for (int j = 0; j < i; j++)
		{
			if (strcmp(input_Command_tokenized[j], "into") == 0)
			{
				into_place = j;
			}
			
		}
		pid_t p1 = 0;
		pid_t p2 = 0;
		pid_t p3 = 0;
		//-----------------------------------  implementing the exit system command   ----------------------
		if (strcmp(input_Command_tokenized[0], "exit") == 0)
		{
			exit(0);
		}
		// reference for "ch" command and "getcwd() from this URL https://man7.org/linux/man-pages/man3/getcwd.3.html
		// https://www.ibm.com/support/knowledgecenter/SSLTBW_2.2.0/com.ibm.zos.v2r2.bpxbd00/rtgtc.htm
		//-----------------------------------  implementing the cd system command   ----------------------
		else if (strcmp(input_Command_tokenized[0], "cd") == 0)
		{
			if (input_Command_tokenized[1]==NULL) // in case user enter cd following by empty line
			{
				chdir("HOME");
			}
			else
			{			
				srcDir = getcwd(buf, sizeof(buf));
				dir = strcat(srcDir, "/");
				destDir = strcat(dir, input_Command_tokenized[1]);
				chdir(destDir);
				printf("%s", destDir);
			}
		}	
		//-----------------------------------  implementing the newpath system command   ----------------------
		else if (strcmp(input_Command_tokenized[0], "newpath") == 0)
		{
			char *new_path;
			if (input_Command_tokenized[1] != NULL)
			{
				if (setenv("PATH", input_Command_tokenized[1], 1) == 0)
				{
					if ((new_path = getenv("PATH")) != NULL)
					{
						printf("New Path: %s", new_path);
					}
				}
				else
				{
					printf("Adding the the new path was unsuccessful!");
				}
			}
			else
			{
				printf("Please restart the Mahsa_shell and enter the new path as an argument to be added to the PATH!");
			}
		}
		//-----------------------------------  implementing the Redirect system command   ----------------------
		else if (strcmp(input_Command_tokenized[0], "redirect") == 0)
		{
			char *command_1[100];
			char *command_2[100];
			if (((into_place + 1) >= 3) && (i >= 4) && (strcmp(input_Command_tokenized[i - 1], "into") != 0))
			{

				for (int k = 0; k < (into_place - 1); k++)
				{ 																// extracting the first command after "redirect"
					command_1[k] = input_Command_tokenized[k + 1];
				}
				command_1[into_place - 1] = NULL;
				int l = 0;
				for (int l = 0; l < (i - into_place); l++)
				{ 																// extracting the second command after "into"
					command_2[l] = input_Command_tokenized[l + into_place + 1];
				}
				
				// I have used the following reference for the pipe() syntax coding  -> http://www.rozmichelle.com/pipes-forks-dups/
				p1 = fork(); // create child process that is a clone of the parent
				if (p1 < 0)
				{
					printf("\nfork error");
				}
				else if (p1 == 0)
				{
					int pipefd[2]; // an array that will hold two file descriptors
					if (pipe(pipefd) < 0) // populates fds with two file descriptors
					{ 
						printf("\nPipe error!");
					}
					p2 = fork(); // create Grandchild
					if (p2 < 0)
					{
						printf("\nfork error (child 2)");
					}
					else if (p2 == 0)   	// executing Grandchild
					{
						close(pipefd[0]);
						dup2(pipefd[1], 1); 
						if (execvp(command_1[0], command_1) < 0)
						{															
							printf("\nEXECVP() error"); // A process that execvp() cannot find.
							exit(0); 
						} 
					}
					else		//back to child process
					{ 
						dup2(pipefd[0], 0);   // fds[0] (the read end of pipe) donates its data to file descriptor 0
						close(pipefd[1]);     // file descriptor no longer needed in child since stdin is a copy
						if (execvp(command_2[0], command_2) < 0)
						{
							printf("\nEXECVP() error"); // A process that execvp() cannot find.
							exit(0);
						}
					}
				}
				else
				{
					wait(NULL);  // A call to wait() blocks the calling process until one of its child processes exits
				}
			}
			else
			{
				printf("/////////////// Command Syntax Error  /////////////////////\nThe keyword “redirect” in first position without the keyword “into” \n");         
				printf("OR\nkeyword 'into' in the last position\nplease enter the redirect command in the correct format");
			}
		}
		else
		{ //----------  implementing all other system commands (except the exit, ch, newpath, redirect  ----------------------

			// Forking a child
			pid_t p3 = fork();
			if (p3 < 0)
			{
				printf("\nCould not fork the child process");
			}
			else if (p3 == 0)
			{
				if (execvp(input_Command_tokenized[0], input_Command_tokenized) == 1)
				{
					perror("Executing the regular system commands");
					_exit(1);
				}
			}
			else
			{
				wait(0);
			}
		}
	} // end of while(1)
	return 0;
} //end of main
