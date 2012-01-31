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
  char *argv[BUFSIZE];
  char * profile[BUFSIZE];
	e_variables=(struct env_var *)malloc(sizeof(struct env_var)*256);
  // read profile
  read_profile(profile);

  // init msh
  // make the current directory is the home assigned in the profile
  init_sh(profile);

  while (1)
  {
    // print the prompt which is settled in the profile
    print_prompt_sign(profile);

    read_cmdline(cmdline);

    // the "exit" will break the while loop and exit normally
    if (strcmp("exit", cmdline) == 0)
      exit(0);

    execute(cmdline, argv);
  }
  puts("Bye");
  return 0;
}

