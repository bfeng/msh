/*
 * msh.h
 *
 *  Created on: Jan 24, 2012
 *      Author: Bo Feng
 */

#ifndef MSH_H_
#define MSH_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define BUFSIZE 256

struct entry {
	char * var_name;
	char * var_value;
};
struct env_var
{
	char* name;
	int value;
};
struct env_var *e_variables;

void point5(char * cmdline);
void change_dir(char *path);
void init_sh(char **profile);
void parse_cmdline(char *cmdline, char **argv);
void print_prompt_sign(char **profile);
int read_profile(char ** profile);
void read_cmdline(char * cmdline);
void execute(char *cmdline, char **argv);
int childexec(char ** argv);

#endif /* MSH_H_ */
