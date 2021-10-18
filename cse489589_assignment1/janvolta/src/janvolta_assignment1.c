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
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>

#include "../include/global.h"
#include "../include/logger.h"

#define TRUE 1 
#define MSG_SIZE 256
#define BUFFER_SIZE 256
#define BACKLOG 5
#define STDIN 0
#define TRUE 1
#define CMD_SIZE 100
#define PORT 3490
#define ip_addr_server "128.205.36.46"
#define port_addr_server "4545"
/**
 * functions and methods 
 */

void server_start(int port);
int connect_to_host(char *server_ip, char* server_port);
void client_start();  
/**

 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argut list
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
	  
          int port_number = atoi(argv[2]);
	  printf("the port number is: %d \n", port_number); 
	  server_start(port_number); 	  
	  
	}

 	/* this is if we want the client to run */
	if(strcmp(argv[1], "c") == 0){
	  char host[256];
	
	  hostname = gethostname(host, sizeof(host)); 
          // https://ubmnc.wordpress.com/2010/09/22/on-getting-the-ip-name-of-a-machine-for-chatty/ and https://www.tutorialspoint.com/c-program-to-display-hostname-and-ip-address
	  host_entry = gethostbyname(host);  
	  //IP = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0])); 
	  printf("Current host name: %s\n", host); 
	  //printf("Host IP: %s\n", IP); 
	  printf("\n");

	  client_start();  
	}

	printf("Exiting");
	return 0;
}

/**********************************************************************************************************/

// starting server function
// stones is going to be the server .. IP ADDRESS 128.205.36.46 & PORT number 4545
void server_start(int port){
  printf("testing \n");
  char port_char[4];
  sprintf(port_char,"%d",port);
  int server_socket, head_socket, selret, sock_index, fdaccept=0, caddr_len; 
  struct sockaddr_in client_addr; 
  struct addrinfo hints, *res; 
  fd_set master_list, watch_list; 
  
  memset(&hints, 0, sizeof(hints)); 
  hints.ai_family = AF_INET; 
  hints.ai_socktype = SOCK_STREAM; 
  hints.ai_flags = AI_PASSIVE; 
  /* Fill up address structures */ 
  if (getaddrinfo(NULL, "4545", &hints, &res) != 0){
    perror("getaddrinfo failed"); 
  }
  
  /* Socket */
  server_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  if(server_socket < 0)
    perror("Cannot create socket");
  
  /* Bind */
  if(bind(server_socket, res->ai_addr, res->ai_addrlen) < 0 )
    perror("Bind failed");

  freeaddrinfo(res);
  
  /* Listen */
  if(listen(server_socket, BACKLOG) < 0)
    perror("Unable to listen on port");
  
  /* ---------------------------------------------------------------------------- */
  
  /* Zero select FD sets */
  FD_ZERO(&master_list);
  FD_ZERO(&watch_list);
  
  /* Register the listening socket */
  FD_SET(server_socket, &master_list);
  /* Register STDIN */
  FD_SET(STDIN, &master_list);
  
  head_socket = server_socket;
  
  while(TRUE){
    memcpy(&watch_list, &master_list, sizeof(master_list));
    
    printf("\n[PA1-Server@CSE489/589]$ ");
    fflush(stdout);
    
    /* select() system call. This will BLOCK */
    selret = select(head_socket + 1, &watch_list, NULL, NULL, NULL);
    if(selret < 0)
      perror("select failed.");
    
    /* Check if we have sockets/STDIN to process */
    if(selret > 0){
      /* Loop through socket descriptors to check which ones are ready */
      for(sock_index=0; sock_index<=head_socket; sock_index+=1){
        
        if(FD_ISSET(sock_index, &watch_list)){
          
          /* Check if new command on STDIN */
          if (sock_index == STDIN){
            char *cmd = (char*) malloc(sizeof(char)*CMD_SIZE);
            
            memset(cmd, '\0', CMD_SIZE);
            if(fgets(cmd, CMD_SIZE-1, stdin) == NULL) //Mind the newline character that will be written to cmd
              exit(-1);
            
            printf("\nI got: %s\n", cmd);
            
            //Process PA1 commands here ...
            
            free(cmd);
          }
          /* Check if new client is requesting connection */
          else if(sock_index == server_socket){
            caddr_len = sizeof(client_addr);
            fdaccept = accept(server_socket, (struct sockaddr *)&client_addr, &caddr_len);
            if(fdaccept < 0)
              perror("Accept failed.");
            
            printf("\nRemote Host connected!\n");                        
            
            /* Add to watched socket list */
            FD_SET(fdaccept, &master_list);
            if(fdaccept > head_socket) head_socket = fdaccept;
          }
          /* Read from existing clients */
          else{
            /* Initialize buffer to receieve response */
            char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
            memset(buffer, '\0', BUFFER_SIZE);
            
            if(recv(sock_index, buffer, BUFFER_SIZE, 0) <= 0){
              close(sock_index);
              printf("Remote Host terminated connection!\n");
              
              /* Remove from watched list */
              FD_CLR(sock_index, &master_list);
            }
            else {
              //Process incoming data from existing clients here ...
              
              printf("\nClient sent me: %s\n", buffer);
              printf("ECHOing it back to the remote host ... ");
              if(send(fdaccept, buffer, strlen(buffer), 0) == strlen(buffer))
                printf("Done!\n");
              fflush(stdout);
            }
            
            free(buffer);
          }
        }
      }
    }
  }

}


/* CLIENT SIDE!! */ 

int connect_to_host(char *server_ip, char *server_port) 
{
  int fdsocket; 
  struct addrinfo hints, *res; 
  
  /* Set up hints structure */ 
  memset(&hints, 0, sizeof(hints)); 
  hints.ai_family = AF_INET; 
  hints.ai_socktype = SOCK_STREAM; 
  
  /* Fill up address structures */ 
  if(getaddrinfo(server_ip, server_port, &hints, &res) != 0){
    perror("Failed to create socket"); 
  }

  /* socket */ 
  fdsocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol); 
  if(fdsocket < 0){
    perror("Failed to create socket!"); 
  }

  /* Connect */ 
  if(connect(fdsocket, res->ai_addr, res-> ai_addrlen) < 0){
    perror("Connect Failed"); 
  }

  freeaddrinfo(res); 
  return fdsocket; 
  
}

void client_start(char *host_ip){
 
  int server; 
  server = connect_to_host("128.205.36.46", "4545"); 
  while(TRUE){
    printf("\n[PA1-Client@CSE489/589]$ ");
    fflush(stdout);
    
    char *msg = (char*) malloc(sizeof(char)*MSG_SIZE);
    memset(msg, '\0', MSG_SIZE);
    if(fgets(msg, MSG_SIZE-1, stdin) == NULL) //Mind the newline character that will be written to msg
      exit(-1);
    
    printf("I got: %s(size:%d chars)", msg, strlen(msg));
    
    printf("\nSENDing it to the remote server ... ");
    if(send(server, msg, strlen(msg), 0) == strlen(msg))
      printf("Done!\n");
    fflush(stdout);
    
    /* Initialize buffer to receieve response */
    char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
    memset(buffer, '\0', BUFFER_SIZE);
    
    if(recv(server, buffer, BUFFER_SIZE, 0) >= 0){
      printf("Server responded: %s", buffer);
      fflush(stdout);
    }
  }
}