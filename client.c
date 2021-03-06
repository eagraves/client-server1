/*
 *  client.c
 *
 *  Modified by Evan Graves on 2/25/13
 * 
 *  Client Program
 *  Send a string to a server using TCP or UDP
 *
 */


// Standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Socket-related libraries
#include <sys/types.h>
#include <sys/socket.h> 
#include <netdb.h>

#define MAX_LINE_SIZE 50

void print_usage(){
  fprintf(stderr, "Usage: ./client [hostname] [port] [connection type]\n");
  fprintf(stderr, "Example: ./client 192.168.0.2 4845 UDP\n\n");
}
 
void check_args(int argc, char *argv[]){
  if( argc != 4 ) {
    print_usage();
	exit(1);
  }
}

int main(int argc, char *argv[])
{

  char udp[] = "UDP";
  char tcp[] = "TCP";
  int success; //For error checking
  struct addrinfo hints;
  struct addrinfo *serverInfo;
  
  // Check arguments and exit if they are not well constructed
  check_args(argc, argv);
  
  // Fill the 'hints' structure
  memset( &hints, 0, sizeof(hints) );
  hints.ai_family   = AF_INET;     // Use IPv4 address
  if(strcmp (udp, argv[3]) == 0)
  	{
  	 hints.ai_socktype = SOCK_DGRAM; // UDP sockets
  	}
  else
  	{
    	 hints.ai_socktype = SOCK_STREAM; // TCP sockets
  	}
  
  // Get the address information of the server.
  // argv[1] is the address, argv[2] is the port
  getaddrinfo(argv[1], argv[2], &hints, &serverInfo);
  
  // Create a socket to communicate with the server
  int socketFD = socket( serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol );
	//Error checking
	if (socketFD == -1) {perror("Failed to create socket"); exit(1);}
  

if(strcmp (tcp, argv[3]) == 0)
  {
  // Connect to the server (only for TCP)
   success = connect(socketFD, serverInfo->ai_addr, serverInfo->ai_addrlen);
	//Error checking
	if (success == -1) {perror("Failed to connect to server"); exit(1);}
  }


  
  // Ask for data to send
  char line[MAX_LINE_SIZE];
  printf("Please input the string to send followed by an integer: ");
  fgets(line, MAX_LINE_SIZE + 1, stdin);
  int len = strlen(line);
  
  // Send the data:
  printf("Sending string '%s' to '%s' using port '%s'... ", line, argv[1], argv[2]);

if(strcmp (udp, argv[3]) == 0)
  {
    success = sendto(socketFD, line, len, 0, serverInfo->ai_addr, serverInfo->ai_addrlen); // UDP style.
	//Error checking
	if (success == -1) {perror("Failed to send data"); exit(1);}
  }

else
  {
     success = send(socketFD, line, len, 0); // TCP style. Use sendto() for UDP
	//Error checking
	if (success == -1) {perror("Failed to send data"); exit(1);}
  }


  printf("Done!\n\n");
    
  // Free the address info struct and close the socket
  freeaddrinfo(serverInfo);
  success = close(socketFD);
	//Error checking
	if (success == -1) {perror("Failed to close socketFD"); exit(1);}
  
  return EXIT_SUCCESS;
}
