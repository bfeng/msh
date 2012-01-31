/*
 * test.c
 *
 *  Created on: Jan 27, 2012
 *      Author: Bo Feng
 */

#include "msh.h"

#define SUCCESS 0
#define FAIL  -1

int test_read_profile();

int main(void)
{
	int ok = 0;
	puts("Start testting...");
	if (test_read_profile() == FAIL)
	{
		ok--;
		puts("Failed: function: read_profile()");
	}
	if(ok<0)
    puts("%d function(s) failed!");
  puts("Test finished");
  return 0;
}

int test_read_profile()
{
	char * profile[BUFSIZE];
	int result = read_profile(profile);

  if(sizeof(profile[0])<=0)
    result = -1;

	if (result < 0)
		return FAIL;
	else
		return SUCCESS;
}
