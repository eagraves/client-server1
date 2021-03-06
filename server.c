/*
 *  server.c
 *
 *  Modified by Evan Graves on 2/25/13
 *  
 *  Server program to receive and encrypt a string
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
#define LISTEN_QUEUE 5

void print_usage(){
  fprintf(stderr, "Usage: ./server [port] [connection type]\n");
  fprintf(stderr, "Example: ./server 4845 TCP\n\n");
}
 
void check_args(int argc, char *argv[]){
  if( argc != 3 ) {
    print_usage();
	exit(1);
  }
}

/////////////////////
// MAIN /////////////
int main(int argc, char *argv[])
{

  char udp[] = "UDP";
  char tcp[] = "TCP";
  char space[] = " ";
  int success; 
  struct addrinfo hints;
  struct addrinfo *serverInfo;
  
  // Check arguments and exit if they are not well constructed
  check_args(argc, argv);
  

  // Fill the 'hints' structure
  memset( &hints, 0, sizeof(hints) );
  hints.ai_flags    = AI_PASSIVE;  // Use the local IP address
  hints.ai_family   = AF_INET;     // Use IPv4 address

  if(strcmp (udp, argv[2]) == 0)
  	{
  	 hints.ai_socktype = SOCK_DGRAM; // UDP sockets
  	}
  else
  	{
    	 hints.ai_socktype = SOCK_STREAM; // TCP sockets
  	}


  // Get the address information of the server ('NULL' uses the local address )
  getaddrinfo( NULL , argv[1], &hints, &serverInfo );
  
  // Make a socket (identified with a socket file descriptor)
  int socketFD = socket( serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol );
	//Error checking
	if (socketFD == -1) {perror("Failed to create socket"); exit(1);}

  // Bind to the specified port (in getaddrinfo())
  success = bind( socketFD, serverInfo->ai_addr, serverInfo->ai_addrlen );
	//Error checking
	if (success == -1) {perror("Failed to bind to port"); exit(1);}
  
  // This is only used to be able to reuse the same port
  int yes = 1;
  setsockopt( socketFD, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int) );
  
if(strcmp (tcp, argv[2]) == 0)
  {
  // Start to listen (only for TCP)
  success = listen(socketFD, LISTEN_QUEUE);
	//Error checking
	if (success == -1) {perror("Falied to begin listening"); exit(1);}
  }


  printf("Listening in port %s...", argv[1]);
  fflush(stdout);
  
  // Free the address info struct
  freeaddrinfo(serverInfo);
  
  // Accept a connection (only for TCP) and create a socket for each one

	//UDP only!
  struct sockaddr clientAddr_UDP;
  memset( &clientAddr_UDP, 0, sizeof(clientAddr_UDP) );
	//UDP!

	//TCP only!
  struct sockaddr_storage clientAddr;
	//TCP!

	//TCP only!
  socklen_t clientAddr_size = sizeof(clientAddr);

	//UDP only!
  socklen_t clientAddr_size_UDP = sizeof(clientAddr_UDP);
  int recvFD;
  while (1) {
   	char line[MAX_LINE_SIZE];   //Received String
   	char processed[MAX_LINE_SIZE];   //processed String
	int offset = 0; //Desired offset from client
	int length = 0; //length of bare string WITHOUT Null char


	//Only if TCP, accept connection to client
	if(strcmp (tcp, argv[2]) == 0)
 	 {
		recvFD = accept(socketFD, (struct sockaddr *)&clientAddr, &clientAddr_size);
		if(recvFD == -1 ) {perror("Failed to accept a connection"); exit(1);}
  	 }
  
    	// Receive data

	//If TCP, use recv() function
	int num_bytes_received = 0;
	if(strcmp (tcp, argv[2]) == 0)
 	 {
    		num_bytes_received = recv(recvFD, line, MAX_LINE_SIZE-1, 0);
 	 }

	//If UDP, use recvfrom() function
	if(strcmp (udp, argv[2]) == 0)
  	{
    		num_bytes_received = recvfrom(socketFD, line, MAX_LINE_SIZE-1, 0, (struct sockaddr *)&clientAddr_UDP, &clientAddr_size_UDP);
  	}

	if(num_bytes_received == -1) {perror("Failed to properly receive data"); exit(1);}


	//Split encryption key from string
	int i;
	for (i = 0; i < MAX_LINE_SIZE; i++)
	{
		length = i;
		if(line[i] == ' ')
		{ 
			offset = line[i+1] - '0'; //Get offset/ecryption value but check if double digit below
			if(line[i+2] == '0' || line[i+2] == '1' || line[i+2] == '2' || line[i+2] == '3' || line[i+2] == '4' || line[i+2] == '5' || line[i+2] == '6' || line[i+2] == '7' || line[i+2] == '8' || line[i+2] == '9')
			{ offset = offset *10; offset += line[i+2] - '0';}
			break;

		}
		
		processed[i] = line[i]; //save string in processed[]. Has not been encrypted yet though
	}


	//Shift string by offset/encryption value
	int j;
	for (j = 0; j < length; j++)
	{
		processed[j] = 'a' + ((processed[j] - 'a' + offset) % 26);
	}
	processed[j] = '\0'; //Place a null char at the end of this string

  
    	// Print received data
    	printf("\n   Received String: '%s'\n\n", processed);
    	printf("Listening in port %s...", argv[1]);
    	fflush(stdout);
  
   	 // Close the socket associated to this client
	if(strcmp (tcp, argv[2]) == 0)
  	{
    	success = close(recvFD);
	    if(success == -1) {perror("Failed to close socket"); exit(1);}
  	}

  }

  // Close the socket that was used to listen (TCP only)
if(strcmp (tcp, argv[2]) == 0)
  {
  success = close(socketFD);
	if(success == -1) {perror("Failed to close socket"); exit(1);}
  }

  
  return EXIT_SUCCESS;
}
