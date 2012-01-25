/*
 * main.c
 *
 *  Created on: Jan 21, 2012
 *      Author: Bo Feng
 */

#include "msh.h"

int main(void)
{
	// trap the signal handler
	// ...

	// init variables
	char cmdline[BUFSIZE];
	char * profile[BUFSIZE];
	pid_t childpid;
	int status, options = 0;

	// read profile
	read_profile(profile);

	while (1)
	{
		printf("! "); // print the prompt which is settled in the profile
		fflush(stdout); // flush in case the output is cached by os

		read_cmdline(cmdline);

		if (strcmp("exit", cmdline) == 0) // the "exit" will break the while loop and exit normally
			break;

		childpid = fork();
		if (childpid == -1)
		{
			perror("Cannot proceed. fork() error");
			return 1;
		}
		if (childpid == 0)
		{
			childexec(cmdline);
			return 0;
		}
		else
		{
			waitpid(childpid, &status, options);
		}
	}
	puts("Bye");
	return 0;
}

