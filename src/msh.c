/*
 * msh.c
 *
 *  Created on: Jan 25, 2012
 *      Author: Bo Feng
 */

#include "msh.h"

/**
 * This function will parse the user's input into a char-array.
 */
void read_cmdline(char * cmdline)
{
	int len = 0;
	char c;
	while ((c = getchar()) != '\n')
		cmdline[len++] = c;
	cmdline[len] = '\0';
}

/**
 * This function is used to execute the child's process
 */
int childexec(char * cmdline)
{
	char * c = cmdline;
	char ret[100];
	char *cmd_array[10];
	int index = 0;
	while (*c != '\0')
	{
		if (*c == ' ')
		{
			cmd_array[index] = (char *) malloc(sizeof(char) * strlen(ret) + 1);
			strncpy(cmd_array[index], ret, strlen(ret));
			strncat(cmd_array[index], "\0", 1);
			index++;
		}
		else
		{
			strncat(ret, c, 1);
		}
		c++;
	}

	execv("/bin/ls", (char **) cmd_array);

	return 0;
}

/**
 * This function is used to read the configuration from a file into a variable-array which will be used later in the whole program.
 */
void read_profile(char ** profile)
{
	// read the profile

	// copy the configuration to the parameter which will be used later in the shell

}
