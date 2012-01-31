/*
 * msh.c
 *
 *  Created on: Jan 25, 2012
 *      Author: Bo Feng
 */

#include "msh.h"

void parse_cmdline(char *cmdline, char **argv)
{
  while(*cmdline !='\0')
  {
    while(*cmdline == ' ' || *cmdline == '\t' ||
          *cmdline == '\n')
      *cmdline++ = '\0';
    *argv++ = cmdline;
    while(*cmdline != '\0' && *cmdline != ' ' &&
          *cmdline != '\t' && *cmdline != '\n')
      cmdline++;
  }
  *argv = '\0';
}

char * read_var(char ** profile, char * var_name)
{
  char * result = NULL;
  char ** line = profile;
  while (*line)
  {
    struct entry par;
    char *c = *line;
    int counter = 0;
    while (c++)
    {
      counter++;
      if (*c == '=')
      {
        par.var_name = (char *) malloc((counter + 1) * sizeof(char));
        strncpy(par.var_name, *line, counter);
        par.var_name[counter] = '\0';

        par.var_value = (char *) malloc(strlen(c) - 1);
        strncpy(par.var_value, c + 1, strlen(c) - 1);
        par.var_value[strlen(c) - 2] = '\0';

        break;
      }
    }
    if (par.var_name != NULL)
    {
      if (strcmp(par.var_name, var_name) == 0)
      {
        result = par.var_value;
        return result;
      }
      else
      {
        line++;
        continue;
      }
    }
    else
      break;
  }
  return NULL;
}

void change_dir(char *path)
{
  if(path==NULL)
  {
    path = getenv("HOME");
  }
  if(chdir(path)==0)
  {
    // preserve for other use
  }
  else
    perror("init error");
}

void init_sh(char **profile)
{
  char *path = read_var(profile, "HOME");
  if(chdir(path)==0)
  {
    setenv("HOME", path, 1);
  }
  else
    perror("init error");
}

void print_prompt_sign(char ** profile)
{
  char * sign = read_var(profile, "SIGN");
  printf("%s ", sign);
  fflush(stdout); // flush in case the output is cached by os
}

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

int handle_builtin(char **argv)
{
  if(strcmp(argv[0], "cd") == 0)
  {
    if(argv[1])
      change_dir(argv[1]);
    return 0;
  }
  return 1;
}

void execute(char *cmdline, char **argv)
{
  if(strlen(cmdline)<=0)
    return;

  int out_type = 0; // 0 for stdout; 1 for redirect; 2 for pipeline

  char *childcmd[2];
  childcmd[0] = cmdline;

  while(*cmdline != '>' && *cmdline != '|' &&
        *cmdline != '\0' && *cmdline != '\n')
  {
    cmdline++;
  }
  if(*cmdline=='>')
  {
    out_type = 1;
    *cmdline++ = '\0';
    childcmd[1] = cmdline;
  }
  else if(*cmdline=='|')
  {
    out_type = 2;
    *cmdline++ = '\0';
    childcmd[1] = cmdline;
  }

  int n = strlen(childcmd[0]) - 1;
  while(n>0)
  {
	  if(childcmd[0][n]==' ')
	  {
		  childcmd[0][n]='\0';
	  }
	  else
		  break;
	  n--;
  }

  while(*childcmd[1] != '\0')
  {
	  if(*childcmd[1]==' ')
		  childcmd[1]++;
	  else
		  break;
  }

  parse_cmdline(childcmd[0], argv);

  int ret = handle_builtin(argv);
  if(ret==0)
    return;

  pid_t childpid;
  int status, options = 0;

  childpid = fork();
  if (childpid == -1)
  {
    perror("Cannot proceed. fork() error");
    exit(1);
  }
  else if (childpid == 0)
  {
    // This is child's process.
    if(out_type==0)
    {
      childexec(argv);
    }
    else if(out_type==1)
    {// output redirection here
      int fd;
      close(STDOUT);
      fd = open(childcmd[1], O_APPEND);
      if(fd<0)
      {
        fd = creat(childcmd[1], 0666);
        if(fd<0)
        {
          perror("create file failed");
          exit(-1);
        }
      }
      dup(fd);
      childexec(argv);
      close(fd);
    }
    else if(out_type==2)
    {// pipeline here
      int fd[2];
      pipe(&fd[0]);
      if(fork()!=0)
      {
        close(fd[0]);
        close(STDOUT);
        dup(fd[1]);
        childexec(argv);
        close(fd[1]);
        puts("\n");
      }
      else
      {
        close(fd[1]);
        close(STDIN);
        dup(fd[0]);
        parse_cmdline(childcmd[1], argv);
        childexec(argv);
        close(fd[0]);
      }
    }
  }
  else
  {
    waitpid(childpid, &status, options);
    if (!WIFEXITED(status))
      printf("Parent: child has not terminated normally.\n");
  }
}

/**
 * This function is used to execute the child's process
 */
int childexec(char ** argv)
{
  if(execvp(*argv, argv) < 0)
  {
    perror("ERROR: exec failed ");
    exit(1);
  }
  return 0;
}

/**
 * This function is used to read the configuration from a file into a variable-array which will be used later in the whole program.
 */
int read_profile(char ** profile)
{
  // read the profile
  // copy the configuration to the parameter which will be used later in the shell
  static const char profile_path[] = "./profile.src";
  FILE * file = fopen(profile_path, "r");
  if (file != NULL)
  {
    char line[128];

    int i = 0;
    while (fgets(line, sizeof(line), file) != NULL)
    {
      profile[i] = (char *) (malloc(sizeof(line)));
      strcpy(profile[i], line);
      i++;
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
