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

int read_profile(char ** profile);
void read_cmdline(char * cmdline);
int childexec(char * cmdline);

#endif /* MSH_H_ */
