/**
 * @janvolta_assignment1
 * @author  jan voltaire vergara <janvolta@buffalo.edu>, Ze Ming Zhang <zemingzh@buffalo.edu>, Jon Choi <jmchoi@buffalo.edu>, Jimmy Holtzman <jholtzma.edu> 
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
#include <netinet/in.h>
#include <arpa/inet.h> 
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

#include <stdbool.h>
#define TRUE 1 
#define FALSE 0
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
void login_initial_state(bool is_initial, int port);
bool ip_valid(char *ip); 

/**

 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argut list
 * @return 0 EXIT_SUCCESS
 */

struct client_message {
	char ip[32];
	char command[20];
	char data[256];
};


struct ls_element {
	char ls_hn[40];
	int ls_port;	
	int ls_id;	
	int rcv_msg;	
	int fd_socket;
	int snd_msg;	
	char status[20];
	char ip[32];
	struct ls_element *next;
};
struct message {
	char ip[32];
	char command[20];
	char data[256];
	struct ls_element ls;
}; 

struct  ls_element *ls_init(struct ls_element *next)	{
	struct ls_element *ls= malloc(sizeof(struct ls_element));
	ls->ls_id = 1;
	ls->next = next;
	return ls;
}
void ls_set_all(
	char ls_hn[40],
	int ls_post,	
	int ls_id,	
	int rcv_msg,	
	int fd_socket,
	int snd_msg,	
	char status[20],
	char ip[32],
	struct ls_element *next,
	struct ls_element *temp
	) {
	
	strcpy(temp->ls_hn, ls_hn);         	
	temp->ls_port	 =  ls_post;	
	temp->ls_id	     =    ls_id;	
	temp->rcv_msg	 =  rcv_msg;	
	temp->fd_socket  =     fd_socket;
	temp->snd_msg	 =  snd_msg;	
	strcpy(temp->status,status);
	temp->     next  =      next;
	strcpy(temp->ip,ip);
}

void copy(struct ls_element *first, struct ls_element *second){
	ls_set_all(	
		first->ls_hn
		,first->ls_port	
		,first->ls_id	
		,first->rcv_msg	
		,first->fd_socket
		,first->snd_msg	
		,first->status
		,first->ip
		,second->next		
		,second
		);
}
void swap(struct ls_element *first, struct ls_element *second){
	struct ls_element temp;
	copy (first,&temp);			
	copy (second,first);
	copy (&temp,second);
}
void print_list(struct ls_element ls){
	cse4589_print_and_log("%-5d%-35s%-20s%-8d\n",
		ls.ls_id,
		ls.ls_hn,
		ls.ip,
		ls.ls_port
		);
}
void ip_address(){
    char hostname[256];
    char *IP;
    struct hostent *client;
   
  
    //transfers it to host buffer
    if(gethostname(hostname, sizeof(hostname)) == 0){ // success
      cse4589_print_and_log("\n[IP:SUCCESS]\n");
      client = gethostbyname(hostname);
      IP = inet_ntoa(*((struct in_addr*)
                           client->h_addr_list[0]));
      cse4589_print_and_log("IP:%s\n",IP);
      cse4589_print_and_log("[IP:END]\n"); 
    }
    else{
      cse4589_print_and_log("\n[IP:ERROR]\n"); 
    }
    
  
    return; 
  
}
void print_statistics(struct ls_element ls){
  // these are place holders 
	cse4589_print_and_log("%-5d%-35s%-8d%-8d%-8s\n", ls.ls_id, ls.ls_hn, ls.snd_msg, ls.rcv_msg, ls.status);
}

struct ls_element *remove_id(struct ls_element *cur,int socket_id){
	if (cur == NULL) exit(-1);
	
	if (cur->fd_socket == socket_id) {return cur->next;} 
	struct ls_element *top = cur;	
	struct ls_element *prev = NULL;
	while (cur != NULL ) {
		if (cur->fd_socket == socket_id) {
			prev->next = cur->next;
			break;
		} 
		prev = cur;		
		cur = cur->next;
	} 
	return top;

}
void print_full_list(struct ls_element *ls) {
	struct ls_element *cur = ls; 
	while (cur != NULL) {
		print_list(*cur);
		cur = cur->next;
	}
}
void  ls_sort(struct ls_element *ls){
	struct ls_element *temp;
	struct ls_element *temp2;
	bool swaped = false;
	do
	{
		swaped = false;
		temp = ls;
		while ( temp->next != temp2 )  {
			if (temp->ls_port > temp->next->ls_port ) {
				swap(temp,temp->next);		
			}
		}
		temp2 = temp->next;
	}
	while (temp != NULL) ;
	
}
int split(char delim,char *input,char *output){
	int i;
	for ( i = 0; i != '\0' && i != delim ; i++){
		output[i] = input[i];
	}
	return i;	
}
bool ip_valid(char *ip){
	struct sockaddr_in sock;
	return inet_pton(AF_INET, ip,&sock.sin_addr) ? true: false;
}

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
	  int port_number = atoi(argv[2]);
		client_start(port_number);  
	}

	printf("Exiting");
	return 0;
}
char author[250] = "I, janvolta, have read and understood the course academic integrity policy.\n\0"; 

bool is_port(char *value){
	for (int i = 0;i != '\0'; i++) {
		if (value[i] <= 9 && value[i] >= 0) return false;
	}

	return true;
}
void set_message(char *command,char *ip,char *msg,struct message *m){
	strcpy(m->command,command);
	strcpy(m->ip,ip);
	strcpy(m->data,msg);

}

/**********************************************************************************************************/

// starting server function
// stones is going to be the server .. IP ADDRESS 128.205.36.46 & PORT number 4545
void server_start(int port){
	
	char port_char[10];
	memset(port_char,'\0', 10); 
	sprintf(port_char,"%d",port);

	struct client_message recieve_mes;
	struct message server_mes;

	int server_socket, head_socket, selret, sock_index, fdaccept=0, caddr_len; 
	struct sockaddr_in client_addr; 
	struct addrinfo hints, *res; 
	fd_set master_list, watch_list; 
	struct ls_element *server_ls = NULL;
	struct ls_element send_ls; 
	memset(&hints, 0, sizeof(hints)); 
	hints.ai_family = AF_INET; 
	hints.ai_socktype = SOCK_STREAM; 
	hints.ai_flags = AI_PASSIVE; 
  /* Fill up address structures */ 
	if (getaddrinfo(NULL, port_char, &hints, &res) != 0){
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

	//printf("[PA1-Server@CSE489/589]$ "); 
	while(TRUE){
		printf("[PA1-Server@CSE489/589]$ "); 
		memcpy(&watch_list, &master_list, sizeof(master_list));

		fflush(stdout);

    /* select() system call. This will BLOCK */
		selret = select(head_socket + 1, &watch_list, NULL, NULL, NULL);
		if(selret < 0)
			perror("select failed.");

    /* Check if we have sockets/STDIN to process */
		if(selret > 0){
      /* Loop through socket descriptors to check which ones are ready */
			for(sock_index=0; sock_index<=head_socket; sock_index+=1){
				fflush(stdout);
				memset(&server_mes, '\0', sizeof(server_mes));
				if(FD_ISSET(sock_index, &watch_list)){

          /* Check if new command on STDIN */
					if (sock_index == STDIN){




						char *cmd = (char*) malloc(sizeof(char)*CMD_SIZE);

            if(fgets(cmd, CMD_SIZE-1, stdin) == NULL) { //Mind the newline character that will be written to cmd
            	exit(-1);
            }
          
            //Process PA1 commands here ...
            if(strcmp(cmd, "AUTHOR\n") == 0){
              cse4589_print_and_log("[AUTHOR:SUCCESS]\n"); 
              cse4589_print_and_log(author); 
              cse4589_print_and_log("[AUTHOR:END]\n"); 
            }
            else if(strcmp(cmd, "IP\n") == 0){
	      ip_address();
            }	
	    else if(strcmp(cmd, "PORT\n") == 0){
	      cse4589_print_and_log("[PORT:SUCCESS]\n"); 
	      cse4589_print_and_log("PORT:%s\n",port_char);
	      cse4589_print_and_log("[PORT:END]\n"); 
	    }
            else if(strcmp(cmd,"LIST") == 0 ) {
		//		cse4589_print_and_log("%i\n",fdsocket);
            	//free(cmd);
            }
            else if(strcmp(cmd, "STATISTICS\n") == 0){
            	int i = 0;

            	for (struct ls_element *cur =server_ls ; cur != NULL; cur = cur->next){
            		if (i== 5) break;
            		i++;
            		print_statistics(*cur);
            		

            	}	

            	printf("[STATISTICS:SUCCESS]\n"); 

            	fflush(stdout);
            }
            free(cmd);
          }
          /* Check if new client is requesting connection */
          else if(sock_index == server_socket){
          	caddr_len = sizeof(client_addr);
          	fdaccept = accept(server_socket, (struct sockaddr *)&client_addr, &caddr_len);
          	if(fdaccept < 0)
          		perror("Accept failed.");
		
          	printf("\nRemote Host connected!\n");                        
		//	char ls_hn[40],
		//    int ls_port,	
		//    int ls_id,	
		//    int rcv_msg,	
		//    int fd_socket,
		//    int snd_msg,	
		//	char status,
		//    struct ls_element *next,
		//	struct ls_element *temp
          	char ip[32];
		
          	inet_ntop(AF_INET,&client_addr.sin_addr.s_addr,ip, INET_ADDRSTRLEN);
          	char host[40];
		
          	getnameinfo((struct sockaddr *)&client_addr, caddr_len,host, sizeof(host), 0,0,0);
	
          	struct ls_element *top = malloc(sizeof(struct ls_element));
          	char status[ 10] = "logged-in\0";
          	int id = (server_ls != NULL)? server_ls->ls_id+1 : 1;
          	ls_set_all(
          		host	
          		,ntohs(client_addr.sin_port)
          		,id
          		,0
          		,fdaccept
          		,0
          		,status
          		,ip
          		,server_ls
          		,top	
          		); 

          	server_ls = top;

            /* Add to watched socket list */
          	FD_SET(fdaccept, &master_list);
          	if(fdaccept > head_socket) head_socket = fdaccept;
		//	fflush(stdout);
          }
          /* Read from existing clients */
          else{
            /* Initialize buffer to receieve response */
          	memset(&recieve_mes, '\0', sizeof(recieve_mes)); 
          	if(recv(sock_index, &recieve_mes, sizeof(recieve_mes), 0) <= 0){

          		close(sock_index);
          		server_ls = remove_id(server_ls,sock_index);
          		printf("Remote Host terminated connection!\n");

              /* Remove from watched list */
          		FD_CLR(sock_index, &master_list);
          	}
          	else {
              //Process incoming data from existing clients here ...

          		printf("\n");  
          		if(strcmp(recieve_mes.command,"LIST") ==0 ){
          			int i = 0;
				bool finished = TRUE;
			//	print_list(&server_ls);
		//		print_full_list(server_ls);
				
          			for (struct ls_element *cur =server_ls ; cur != NULL; cur = cur->next){
          				if (i== 5) break;
          				i++;
                  if(strcmp(cur->status,"logged-in") == 0){
                    print_list(*cur);
                    copy(cur,&send_ls);		
                    strcpy(server_mes.command,"LIST");
                    server_mes.ls=	send_ls;
                    if(send(sock_index,&server_mes,sizeof(server_mes),0) == sizeof(server_mes) ){
              //printf("list_sent\n");
                    }
                    else{
                        finished = FALSE;
                        cse4589_print_and_log("[LIST:ERROR]\n");
                        break; 
                      }
                  }
          			}
				if(finished){
				  strcpy(server_mes.command,"LISTEND_S");
				}
				else{
				  strcpy(server_mes.command,"LISTEND_F");
				}
          			if(send(sock_index, &server_mes, sizeof(server_mes), 0) == sizeof(server_mes)) {
          				printf("Done!\n");
          			}
          			fflush(stdout);
          		}
          		else if (strcmp(recieve_mes.command,"LOGOUT") == 0){
			  //do the forloopy loopy thingy
          			for(struct ls_element *cur = server_ls; cur!= NULL; cur = cur->next){
          				if(cur->fd_socket == sock_index){
          					strcpy(cur->status, "logged-out");
          					break;
          				}
          			} 
          			fflush(stdout); 
          		}
          		else if(strcmp(recieve_mes.command, "LOGIN") == 0){
          			for(struct ls_element *cur = server_ls; cur!= NULL; cur = cur->next){
          				if(cur->fd_socket == sock_index){
          					strcpy(cur->status, "logged-in");
          					break;
          				}
          			} 
          			fflush(stdout); 
          		}
          		else if (strcmp(recieve_mes.command,"SEND") == 0){
          			printf("ACCEPT\n"); 
          			char recv_ip[32],send_ip[32];
          			int send_socket_id = 0;
          			for (struct ls_element *cur = server_ls; cur != NULL ; cur = cur->next) {
          				if (cur->fd_socket == sock_index){
          					strcpy(send_ip,cur->ip);	
          					cur->snd_msg++;
          					break;
          				}
          			}
          			for (struct ls_element *cur = server_ls; cur != NULL ; cur = cur->next) {
          				if (strcmp(cur->ip,recieve_mes.ip) == 0){
          					strcpy(recv_ip,cur->ip);	
          					cur->rcv_msg++;
          					//if(cur->status,
          					send_socket_id = cur->fd_socket;
          					break;
          				}
          			}
          			bool blocked = true;
          			if (!blocked) {
          				continue;	
          			} 
          			else {
          				strcpy(server_mes.command,"MESSAGE");
          				strcpy(server_mes.ip,send_ip);
          				strcpy(server_mes.data,recieve_mes.data);
					printf("what the client messaged: %s\n",recieve_mes.data);
          				if (send(send_socket_id, &server_mes,sizeof(server_mes),0) ==  sizeof(server_mes)) {
          					cse4589_print_and_log("[RELAYED:SUCCESS]\n");
          					cse4589_print_and_log("%s to %s of(%s)\n",send_ip,recv_ip,server_mes.data);
						//	if(send(send_socket_id, &server_mes,sizeof(server_mes),0) ==  sizeof(server_mes)) 
						//{
						//	}
          				}
          				else {
          					cse4589_print_and_log("[RELAYED:FAILED]\n");
          				}
          			} 
          			fflush(stdout); 
					//DO BLOCK_LIST
          		} 

          		else if (strcmp(recieve_mes.command,"BROADCAST") == 0) {
          			for (struct ls_element *cur = server_ls; cur != NULL;  cur = cur->next  )	{
          				set_message("MSG",cur->ip,recieve_mes.data,&server_mes);	
          				if (send(cur->fd_socket,&server_mes,sizeof(server_mes),0) == sizeof(server_mes)) {
					//	cse4589_print_and_log("%s to %s of(%s)\n",recieve_mes.ip,cur->ls_id,server_mes.data);

          					cse4589_print_and_log("[BROADCAST:SUCCESS]\n");

          				}else {
          					cse4589_print_and_log("[BROADCAST:FAILED]\n");
          				}
				//	print_list(*cur);
          			}				

          		}
          	}
			//fflush(stdout);
          }
        }
      }
    }
  }

}

struct client_message client_mess; 
int server; 
/* CLIENT SIDE!! -------------------------------------------------------------------------------------------------------*/ 
void login_initial_state(bool is_initial, int portnumber){
	while(TRUE){
		char *msg = (char*) malloc(sizeof(char)*MSG_SIZE);
		printf("\n[PA1-Client@CSE489/589]$ ");
		 //Mind the newline character that will be written to msg
		if(fgets(msg, MSG_SIZE-1, stdin) == NULL) //Mind the newline character that will be written to msg
			exit(-1);
		char arg[100][100];
		int i=0, j = 0;
		for (int a = 0; a <100; a++){
		  for(int b = 0; b < 100; b++){
		    arg[a][b] = '\0'; 
		  }
		}
		for (int n = 0; msg[n] != '\0'; n++){
			if (msg[n] == ' ' || msg[n] == '\n' ){
				i++;
				j = 0;
			}else{
				arg[i][j] = msg[n];
				j++;
			}
		}
		if (arg[0] == NULL) {
			exit(-1);
		}

		free(msg);
		msg = arg[0]; 
		if(strcmp(msg,"LOGIN") == 0){
			if(!is_initial){
				strcpy(client_mess.command, "LOGIN"); 
				if(send(server, &client_mess, sizeof(client_mess), 0) == sizeof(client_mess)){
					cse4589_print_and_log("\n[LOGIN:SUCESS]\n"); 
				}
				else{
					cse4589_print_and_log("\n[LOGIN:ERROR]\n");
				}
				return;
			// ----------------------------------------SEND LIST STUFF BECAUSE ITS REQUIRED 
			}
			else{
			  break; 
			}

		}

    
		else if (strcmp(msg, "EXIT") == 0){
			exit(0); 
		}
		else if (strcmp(msg,"AUTHOR")==0) {
		  cse4589_print_and_log("[AUTHOR:SUCCESS]\n"); 
		  cse4589_print_and_log(author); 
		  cse4589_print_and_log("[AUTHOR:END]\n");

		}
		else if (strcmp(msg,"IP")==0)  {
		  ip_address(); 
		  
		}
		else if (strcmp(msg,"PORT")==0)  {
		  cse4589_print_and_log("[PORT:SUCCESS]\n"); 
		  cse4589_print_and_log("PORT:%d\n",portnumber); 
		  cse4589_print_and_log("[PORT:END]\n");
		}
		


		
	}
	return;
}
int fdsocket = -1; 

int connect_to_host(char *server_ip, char *server_port) 
{
 
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server)); 
    server.sin_family = AF_INET;
    inet_pton(AF_INET, "128.205.36.46", &server.sin_addr);
    server.sin_port = htons(4545);

    if(connect(fdsocket, (struct sockaddr*)&server, sizeof(server)) < 0)
    {
        perror("[LOGIN:ERROR]");
    }
    
  
    return fdsocket;
}

int choose_port(int port){
    struct sockaddr_in client;
    fdsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);// return socket file descriptor
    if(fdsocket < 0)
    {
       perror("\n[LOGIN:ERROR]\n");
       return 0;
    }

    //setting up client socket
    client.sin_family=AF_INET;
    client.sin_addr.s_addr=INADDR_ANY;
    client.sin_port=htons(port);
   
    int options = 1 ;
    //https://linux.die.net/man/3/setsockopt#:~:text=The%20setsockopt%20%28%29%20function%20shall%20set%20the%20option,the%20file%20descriptor%20specified%20by%20the%20socket%20argument.
    setsockopt(fdsocket, SOL_SOCKET, SO_REUSEPORT, &options, sizeof(options));
    if(bind(fdsocket, (struct  sockaddr*) &client, sizeof(struct sockaddr_in)) < 0)
    {
    	printf("\n[LOGIN:ERROR]\n");
    	return 0;
    } 
    return 1;
}

void client_start(int port){
	int server_socket, head_socket, selret, sock_index, fdaccept=0, caddr_len; 
	char *lst_appender = (char*) malloc(500*sizeof(char));
	memset(lst_appender, '\0', 200);
	choose_port(port); 
	bool initial_login_state = TRUE;
	login_initial_state(initial_login_state, port); 
	server = connect_to_host("128.205.36.46", "4545"); 

	fd_set master_list, watch_list; 
	FD_ZERO(&master_list);
	FD_ZERO(&watch_list);
	FD_SET(STDIN,&master_list);
	head_socket = 0 ; 
	strcpy(client_mess.command, "LIST");
	if(send(server,&client_mess,sizeof(client_mess),0)==sizeof(client_mess)){
	  cse4589_print_and_log("\n[LIST:SUCCESS]\n");
	}
	else{
	  cse4589_print_and_log("[LIST:ERROR]\n");
	}

	while(TRUE){

	  //printf("\n[PA1-Client@CSE489/589]$ ");
		fflush(stdout);
		

		FD_ZERO(&master_list);	
		FD_ZERO(&watch_list);

		FD_SET(STDIN,&master_list);
		
		FD_SET(server,&master_list);

		head_socket = server;

		memcpy(&watch_list, &master_list, sizeof(master_list));
		
		//char *msg = (char*) malloc(sizeof(char)*MSG_SIZE);
		//memset(msg, '\0', MSG_SIZE);

		selret = select(head_socket + 1, &watch_list, NULL, NULL, NULL);
		int ip_char_counter;	
		if(selret < 0)  {
			printf("ERROR selret\n");
			exit(-1);
		}
		for (sock_index=0; sock_index <= head_socket; sock_index++ ) {
			if(!FD_ISSET(sock_index,&watch_list)) continue;
			if (sock_index == STDIN) {
			 
				char *msg = (char *) malloc(sizeof(char)*MSG_SIZE); 
				memset(msg, '\0', MSG_SIZE); 
//				printf("\n[PA1-Client@CSE489/589]$ "); 
				fflush(stdout); 
				
				if(fgets(msg, MSG_SIZE-1, stdin) == NULL) //Mind the newline character that will be written to msg
					exit(-1);
				
				char arg[100][100];
				int i=0, j = 0;
				for(int n =0; n < 100; n++){
					for(int j = 0; j < 100; j++){
						arg[n][j] = '\0';
					}
				}

				for (int n = 0; msg[n] != '\0'; n++){
					if (msg[n] == ' ' || msg[n] == '\n' ){
						i++;
						j = 0;
					}else if(msg[n] != '\n' && msg[n] != EOF){
						arg[i][j] = msg[n];
						j++;
					}
				}
				if (arg[0] == NULL) {
					exit(-1);
				}



				free(msg); 
				msg = arg[0]; 
			
				printf("I got: %s(size:%d chars)\n", msg, strlen(msg));
				if (strcmp(msg,"LOGOUT") == 0){
					strcpy(client_mess.command, "LOGOUT"); 
					if (send(server, &client_mess, sizeof(client_mess), 0) == sizeof(client_mess) ){
						cse4589_print_and_log("\n[LOGOUT:SUCCESS]\n");
					}
					else{
						cse4589_print_and_log("[LOGOUT:ERROR]\n"); 
					} 
					login_initial_state(FALSE, port);
					//initial_login_state = FALSE;
					fflush(stdout); 
				
				}else if(strcmp(msg,"IP") == 0){
				  ip_address(); 
				}
				else if (strcmp(msg, "PORT") == 0){
				  cse4589_print_and_log("[PORT:SUCCESS]\n"); 
				  cse4589_print_and_log("PORT:%d\n",port); 
				  cse4589_print_and_log("[PORT:END]\n");
				}
				else if (strcmp(msg,"LIST")==0) {
				   cse4589_print_and_log(lst_appender);
				}else if (strcmp(msg,"AUTHOR") == 0){
				  cse4589_print_and_log("[AUTHOR:SUCCESS]\n"); 
				  cse4589_print_and_log(author); 
				  cse4589_print_and_log("[AUTHOR:END]\n");
        }
				else if (strcmp(msg,"REFRESH") == 0){
          free(lst_appender);
          char *lst_appender = (char*) malloc(200*sizeof(char));
          memset(lst_appender, '\0', 200);
					strcpy(client_mess.command, "LIST");
					if (send(server, &client_mess, sizeof(client_mess),0) == sizeof(client_mess) ) {
						cse4589_print_and_log("\n[LIST:SUCCESS]\n");
					}
					else
					{
						cse4589_print_and_log("[LIST:ERROR]\n");
					}
					fflush(stdout);
				}else if (strncmp(msg,"SEND", 4) == 0) {
					char *ip = arg[1];
					printf("arg[2]%s\n",arg[2]);			
					if(TRUE){
					  printf("WENT THRU SEND");
						strcpy(client_mess.data,arg[2]);
						strcpy(client_mess.ip,ip);
						strcpy(client_mess.command,"SEND");
						printf("size of client send: %d\n",sizeof(client_mess));
						if (send(server,&client_mess,sizeof(client_mess),0) == sizeof(client_mess)){
							printf("%s\n",client_mess.data);
						}		
					}
					else {
						cse4589_print_and_log("[SEND:ERROR]\n");
						cse4589_print_and_log("[SEND:END]\n");
					}

				}else if (strcmp(msg,"BROADCAST")==0) {
					char *mes = arg[1];
					strcpy(client_mess.command,"BROADCAST");
					strcpy(client_mess.data,msg);
					if (send(server,&client_mess,sizeof(client_mess),0) == sizeof(client_mess)) {

						cse4589_print_and_log("[BROADCAST:SUCCESS]");
					}
					else {
						cse4589_print_and_log("[BROADCAST:FAILED]");

					}
				}
				fflush(stdout); 
				//free(msg); 
    			/* Initialize buffer to receieve response */
			}else {
				struct message rec_server_mes;
//se				lret = select(&head_socket + 1, &watch_list, NULL, NULL, NULL);
				
				memset(&rec_server_mes, '\0', sizeof(rec_server_mes));
				if(recv(server, &rec_server_mes, sizeof(rec_server_mes), 0) >= 0){
					if (strcmp(rec_server_mes.command,"LIST") == 0 )	{
						print_list(rec_server_mes.ls);
            if(strlen(lst_appender) == 0){ // first initialization
              sprintf(lst_appender, "%-5d%-35s%-20s%-8d\n\0", rec_server_mes.ls.ls_id, rec_server_mes.ls.ls_hn,rec_server_mes.ls.ip,rec_server_mes.ls.ls_port);
            }
            else{
              char temp[500];
	      memset(&temp, '\0', 500);
              sprintf(temp, "%-5d%-35s%-20s%-8d\n\0", rec_server_mes.ls.ls_id, rec_server_mes.ls.ls_hn,rec_server_mes.ls.ip,rec_server_mes.ls.ls_port);
              strcat(lst_appender, temp); 
            } 
              
						 
					}else if(strcmp(rec_server_mes.command,"LISTEND_S") == 0)  {
						cse4589_print_and_log("[LIST:END]\n");
					}else if(strcmp(rec_server_mes.command,"LISTEND_F")==0){
					  cse4589_print_and_log("[LIST:ERROR]\n");
					}
					
					else if(strcmp(rec_server_mes.command,"MESSAGE") == 0)  {
						cse4589_print_and_log("From(%s):%s\n",rec_server_mes.ip,rec_server_mes.data);
					}
					else{ printf("Server responded: %s", rec_server_mes.data);
//					      return; 
				}
				fflush(stdout);
			}else  {
						printf("yo\n"); //	close(sock_index);
					}	
				}
				fflush(stdout);
			}
		}
	}
//-----------------------------------------------------------------------------------------------------------------------------------------------
	void get_List(){


	}
