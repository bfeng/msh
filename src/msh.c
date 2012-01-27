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
	char ret[BUFSIZE];
	char *cmd_array[10];
	int index, cmd_index;
	index = cmd_index = 0;
	while (*c != '\0')
	{
		if (*c == ' ')
		{
			ret[index] = '\0';

			size_t ret_len = sizeof(ret) / sizeof(char);
			size_t i = 0;
			cmd_array[cmd_index] = (char *) malloc(sizeof(ret) + sizeof(char));
			for (; i < ret_len; i++)
			{
				cmd_array[cmd_index][i] = ret[i];
			}
			cmd_index++;
			index = 0;
		}
		else
		{
			ret[index] = *c;
			index++;
		}
		c++;
	}

	ret[index] = '\0';
	size_t ret_len = sizeof(ret) / sizeof(char);
	size_t i = 0;
	cmd_array[cmd_index] = (char *) malloc(sizeof(ret) + sizeof(char));
	for (; i < ret_len; i++)
	{
		cmd_array[cmd_index][i] = ret[i];
	}
	cmd_index++;
	index = 0;

	execv(cmd_array[0], cmd_array);
	return 0;
}

/**
 * This function is used to read the configuration from a file into a variable-array which will be used later in the whole program.
 */
int read_profile(char ** profile)
{
	// read the profile
	// copy the configuration to the parameter which will be used later in the shell
	static const char profile_path[] = "profile.src";
	FILE * file = fopen(profile_path, "r");
	if (file != NULL)
	{
		char line[128];

		int i = 0;
		while (fgets(line, sizeof(line), file) != NULL)
		{
			profile[i] = (char *)(malloc(sizeof(line)));
			strcpy(profile[i], line);
		}
		fclose(file);
	}
	else
	{
		perror(profile_path);
		return -1;
	}

	return 0;
}
