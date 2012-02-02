/*
 * msh.c
 *
 *  Created on: Jan 25, 2012
 *      Author: Bo Feng
 */

#include "msh.h"

jmp_buf jump_buffer;

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
	 
  signal(SIGINT, ctrl_CHandler);	
  setjmp(jump_buffer); 
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
  point5(cmdline);
  return;
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
void point5(char *input)
{	
	char variables1[150][150];
	char op1[150];
	char destination1[strlen(input)];
	int destinationvalue,index_i=0,found=0,sourcefound=0,destinationfound=0,s_index=0,d_index=0;
	int i=0,j=0,k=0,op=0;
	int valuesofvariables[150];
	char* multi1;
	char* divi;
	char* addition1;
	char* subtraction1;
	char* oper;
	memset(valuesofvariables,0,sizeof(int)*150);

	// Converst Input to Form: //res=$((a*b-c+d/e-f))
	char command[strlen(input)];

	// Remove Spaces from the command if they exist
	while(input[i] != '\0')
	{
		if(input[i]!=' ')
		{
			command[j]=input[i];
			j++;
			i++;
		}
		else
			i++;
	}			
	command[j]='\0';

	// Find Name of Destination Variable Name i.e res=... "destination1"
	j=i=0;
	while(command[i] != '\0')
	{
		if(command[i]=='=')
		{
			destination1[i]='\0';
			i++;
			break;
		}

		else
		{
			destination1[i]=command[i];
			i++;
		}
	}

	// Find Remaining Variables and ops //res=$((a*b-c+d/e-f))
	index_i=0;	
	k=0; op=0;
	while(command[i] != '\0')
	{
		j=command[i];
		if(j=='+' || j=='/' || j=='-' || j=='*')
		{
			variables1[index_i][k]='\0';
			op1[op]=j;
			op++;
			index_i++;
			k=0;		
		}
		else
		{
			if(j!='$' && j!='(' && j!=')')
			{
				variables1[index_i][k]=j;	
				k++;
			}
		}
	  i++;	
	}			
	
	
	variables1[index_i][k]='\0';
	index_i++;
	op1[op]='\0';

	// use variable values , else assign 0
	i=j=0;
	while(i < index_i)
	{
		while(e_variables[j].name != NULL)
		{
			if(!strcmp(e_variables[j].name,variables1[i]) )
			{
				valuesofvariables[i]=e_variables[j].value;
				break;
			}
			j++;
		}

		// If user inputs raw integers at the command line
		if(atoi(variables1[i]))
			valuesofvariables[i]=atoi(variables1[i]);
		j=0;
		i++;
	}

	// Determine the Mathematical operations to be performed if they exist
	multi1=index(command,'*');
	divi=index(command,'/');
	addition1=index(command,'+');
	subtraction1=index(command,'-');
	
	// Case: Math is required 
	if(multi1||divi||addition1||subtraction1)
	{
		// Check Arithmetic is in Linux Format of Command: //res=$((a*b-c+d/e-f))
		if(command[strlen(destination1)+1] == '(' \
				&& command[strlen(command)-1] == ')' )
		{
			// Handle Multiplication and Division first
			while(multi1 || divi)
			{
				// Multiplication Case
				if( ((multi1 && divi) && strlen(multi1) > strlen(divi)) \
				    || ((multi1) && !divi) )
				{				
					// Find the two Variables to perform Multiplication on
					oper=index(op1,'*');
					destinationvalue=valuesofvariables[strlen(op1)-strlen(oper)]*valuesofvariables[strlen(op1)-strlen(oper)+1];
				}

				// Division Case
				else
			  	{	
					oper=index(op1,'/');
					if(!valuesofvariables[strlen(op1)-strlen(oper)+1])
					{
						printf("myshell: error division by zero is prohibited!\n");
						return;
					}						
					destinationvalue=valuesofvariables[strlen(op1)-strlen(oper)]/valuesofvariables[strlen(op1)-strlen(oper)+1];		
				 }

				   // Modify valuesofvariables array and op1 array 
				   for(i=strlen(op1)-strlen(oper);i<strlen(op1);i++)
				   {
					if(op1[i]=='\0')
						break;
					else
						op1[i]=op1[i+1];
				   }
			 	   multi1=index(op1,'*');
				   divi=index(op1,'/');			 
				   valuesofvariables[strlen(op1)-strlen(oper)]=destinationvalue;
			
			           // Update the valuesofvariables	
				   for(i=strlen(op1)-strlen(oper)+1;i<index_i;i++)
					valuesofvariables[i]=valuesofvariables[i+1];			

				   index_i--;
				 
			    } // End MULTIPLICATION/DIVISION

			// Handle Addition and Division 
			while(addition1 || subtraction1)
			{
				// Add Case
				if( ((addition1 && subtraction1) && strlen(addition1) > strlen(subtraction1)) \
				    || ((addition1) && !subtraction1) )
				{				
					// Find the two Variables to perform Multiplication on
					oper=index(op1,'+');
					destinationvalue=valuesofvariables[strlen(op1)-strlen(oper)]+valuesofvariables[strlen(op1)-strlen(oper)+1];
				}

				// Subtract Case
				else
			  	{	
					oper=index(op1,'-');					
					destinationvalue=valuesofvariables[strlen(op1)-strlen(oper)]-valuesofvariables[strlen(op1)-strlen(oper)+1];		
				}

				// Modify valuesofvariables array and op1 array 
				for(i=strlen(op1)-strlen(oper);i<strlen(op1);i++)
				{
					if(op1[i]=='\0')
						break;
					else
						op1[i]=op1[i+1];
				}
			 	addition1=index(op1,'+');
				subtraction1=index(op1,'-');			 
				valuesofvariables[strlen(op1)-strlen(oper)]=destinationvalue;
			
			        // Update the valuesofvariables	
				for(i=strlen(op1)-strlen(oper)+1;i<index_i;i++)
					valuesofvariables[i]=valuesofvariables[i+1];			

				index_i--;
				
				 
			} // End Add/Sub
			printf("result is %d\n",destinationvalue);

			// MATH COMPLETE assign the value to the destination variable:
			// Search if the Destination Variable exists: destination1
			i=0;
			while(e_variables[i].name != NULL)
			{
				if(!strcmp(e_variables[i].name,destination1))
				{
					// Variable name exists just update the value
                                        found=1;
					e_variables[i].value=destinationvalue;
					break;
				}
			 i++;
				
			}

			// Not Found Create New!
			if(!found)
			{
				e_variables[i].name = (char *)malloc(sizeof(char)*strlen(destination1)+1);
				memcpy(e_variables[i].name,(char *)destination1,strlen(destination1)+1);
				strncat(e_variables[i].name,"\0",1);
				e_variables[i].value=destinationvalue;
			}
		}

		else
		{
			printf("myshell: Improper format, try: res=(a*b-c+d/e-f)\n");
			return;
		}	
	}

	// Arithmatic assingment: i.e: x=5 or x=$y
	else
	{
		// Variable Assingment
		if(index(command,'$'))
		{
			//Example: x=$y
		
			printf("In the right block\n");
	
			//Determine if the source Variable exists
			i=0;
			while(e_variables[i].name != NULL)
			{
				if(!strcmp(e_variables[i].name,variables1[0]))
				{
					printf("Found source at %d\n",i);
					sourcefound=1;
					s_index=i;
				}
				
				else if(!strcmp(e_variables[i].name,destination1) )
				{	
					printf("Found dest at %d\n",i);	
					destinationfound=1;
					d_index=i;
				}
				i++;
			}
		
			// Nothing to Assign
			if(!sourcefound)
			{       printf("Source Not Found\n");	
				return;
			}

			// Both found assign the value of destination var to source var
			if(sourcefound && destinationfound)
			{
				printf("both found\n");
				e_variables[d_index].value=e_variables[s_index].value;
				return;
			}
			
			// Source is Found but Destination is not found
			// Destination must be created
			if(sourcefound && !destinationfound)
			{
				//Create 
				e_variables[i].name = (char *)malloc(sizeof(char)*strlen(destination1)+1);
				memcpy(e_variables[i].name,(char *)destination1,strlen(destination1)+1);
				strncat(e_variables[i].name,"\0",1);
				e_variables[i].value=e_variables[sourcefound].value;
				return;			
			}	
		}

		// Integer Assingment
		else
		{
			// Example: x=5
			destinationvalue=atoi(variables1[0]);

			// Search if the Destination Variable exists: destination1
			i=0;
			while(e_variables[i].name != NULL)
			{
				if(!strcmp(e_variables[i].name,destination1))
				{
					// Variable name exists just update the value
                                        found=1;
					e_variables[i].value=destinationvalue;
					break;
				}
			 i++;
			}

			// Not Found Create New!
			if(!found)
			{
				//printf("Adding %s i is %d\n",destination1, i);
				e_variables[i].name = (char *)malloc(sizeof(char)*strlen(destination1)+1);
				memcpy(e_variables[i].name,(char *)destination1,strlen(destination1)+1);
				strncat(e_variables[i].name,"\0",1);
				e_variables[i].value=destinationvalue;
			}	
		}
	}	
    return;
}

void ctrl_CHandler(int param)
{
	
   char alter[150];  
   printf("\n Exit from the shell: Are you sure? [yes/no]: ");
   scanf("%31s", alter);
       
   if((strcmp(alter, "y") == 0) || (strcmp(alter, "Y") == 0) || (strcmp(alter, "yes") == 0)
   || (strcmp(alter, "YES") == 0))
      exit(0);
   else
   longjmp(jump_buffer,1);
}


