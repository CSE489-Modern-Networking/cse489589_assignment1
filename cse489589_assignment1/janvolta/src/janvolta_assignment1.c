/**
 * @janvolta_assignment1
 * @author  jan voltaire vergara <janvolta@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This contains the main function. Add further description here....
 */
#include <stdio.h>
#include <stdlib.h>

#include "../include/global.h"
#include "../include/logger.h"

#define TRUE 1 
#define MSG_SIZE 256
#define BUFFER_SIZE 256

/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */
int main(int argc, char **argv)
{
	/*Init. Logger*/
	cse4589_init_log(argv[2]);

	/*Clear LOGFILE*/
	fclose(fopen(LOGFILE, "w"));
	
	/*Start Here*/
	if(argc != 3){
	  printf("USAGE%s [ip] [port]\n", argv[0]); 
	  exit(-1);
	}
	/* this is if the server is running */ 
	if(strcmp(argv[1], "s") == 0){ // this checks if there is s
	  int port = atoi(argv[2]);
	  printf("The port number is: %d \n", port); 
	}

 	/* this is if we want the client to run */
	if(strcmp(argv[1], "c") == 0){
	  return 0;
	}

	
	return 0;
}
