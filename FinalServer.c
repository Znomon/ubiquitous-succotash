//================================================= file = FinalServer.c ======
//= Notes: =
//= 1) This program conditionally compiles for Winsock and BSD sockets. =
//= Set the initial #define to WIN or BSD as appropriate. =
//=---------------------------------------------------------------------------=
//= Example execution: =
//= Waiting for recvfrom() to complete... =
//= Received from client: "Test cat catalog excat 123!" =
//= Sent to client: "Test dog dogalog exdog 123!" =
//=---------------------------------------------------------------------------=
//= Build: bcc32 FinalServer.c or cl FinalServer.c wsock32.lib for Winsock =
//= gcc FinalServer.c -lsocket -lnsl for BSD =
//=---------------------------------------------------------------------------=
//= Execute: catdogServer =
//=---------------------------------------------------------------------------=
//= Author(s): Kasey Kolyno and Ryan Binder =
//= University of South Florida =
//= Email: (kolyno, #FIXME)@usf.edu =
//=---------------------------------------------------------------------------=
//= History: KFK and #FIXME (11/14/15) - Genesis (from udpServer.c) =
//=============================================================================



#define BSD // WIN for Winsock and BSD for BSD sockets
//----- Include files --------------------------------------------------------
#include <stdio.h> // Needed for printf()
#include <string.h> // Needed for strcpy()
#include <stdlib.h> // Needed for exit() and rand()
#ifdef WIN
#include <windows.h> // Needed for all Winsock stuff
#endif
#ifdef BSD
#include <sys/types.h> // Needed for sockets stuff
#include <netinet/in.h> // Needed for sockets stuff
#include <sys/socket.h> // Needed for sockets stuff
#include <arpa/inet.h> // Needed for sockets stuff
#include <fcntl.h> // Needed for sockets stuff
#include <netdb.h> // Needed for sockets stuff
#endif
//----- Defines --------------------------------------------------------------
#define PORT_NUM 1050 // Arbitrary port number for the server
int checkRequest(char *b); // Switch "cat" to "dog" in buffer b
//===== Main program =========================================================
int main() {
  #ifdef WIN
	WORD wVersionRequested = MAKEWORD(1, 1); // Stuff for WSA functions
	WSADATA wsaData; // Stuff for WSA functions
	#endif
	int server_s; // Server socket descriptor
	struct sockaddr_in server_addr; // Server Internet address
	struct sockaddr_in client_addr; // Client Internet address
	int addr_len; // Internet address length
	char out_buf[4096]; // Output buffer for data
	char in_buf[4096]; // Input buffer for data
	int retcode; // Return code
	#ifdef WIN
	// This stuff initializes winsock
	WSAStartup(wVersionRequested, & wsaData);
  #endif
	// Create a socket
	server_s = socket(AF_INET, SOCK_DGRAM, 0);
	if (server_s < 0) {
		printf("*** ERROR - socket() failed \n");
		exit(-1);
	}
	// Fill-in my socket's address information
	server_addr.sin_family = AF_INET; // Address family to use
	server_addr.sin_port = htons(PORT_NUM); // Port number to use
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // Listen on any IP address
	retcode = bind(server_s, (struct sockaddr * ) & server_addr,
	sizeof(server_addr));
	if (retcode < 0) {
		printf("*** ERROR - bind() failed \n");
		exit(-1);
	}
	//Loop forever
	while (1) {
		// Wait to receive a message from client
		printf("Waiting for recvfrom() to complete... \n");
		addr_len = sizeof(client_addr);
		retcode = recvfrom(server_s, in_buf, sizeof(in_buf), 0, (struct sockaddr * ) & client_addr, & addr_len);
		if (retcode < 0) {
			printf("*** ERROR - recvfrom() failed \n");
			exit(-1);
		}
		printf(" Received from client: %s \n", in_buf);
		// Switch "cat" to "dog" in in_buf and copy in_buf to out_buf
    
    char buffer[400];
    char *test = buffer;
    
    if(checkRequest(in_buf) == 3)
    {
      sprintf(test, "Your username is %s", in_buf);
      strcpy(out_buf, buffer);
    }
    
    
		//strcpy(out_buf, in_buf);

		// Send out_buf to the client using the server socket
		retcode = sendto(server_s, out_buf, (strlen(out_buf) + 1), 0, (struct sockaddr * ) & client_addr, sizeof(client_addr));
		if (retcode < 0) {
			printf("*** ERROR - sendto() failed \n");
			exit(-1);
		}
		printf(" Sent to client: %s \n", out_buf);
	}
	// Close all open sockets
	#ifdef WIN
	retcode = closesocket(server_s);
	if (retcode < 0) {
		printf("*** ERROR - closesocket() failed \n");
		exit(-1);
	}
  #endif
  #ifdef BSD
	retcode = close(server_s);
	if (retcode < 0) {
		printf("*** ERROR - close() failed \n");
		exit(-1);
	}
  #endif
  #ifdef WIN
	// This stuff cleans-up winsock
	WSACleanup();
  #endif
	// Return zero and terminate
	return (0);
}

int checkRequest(char * b) {
	int n; // Length of string b
	int i; // Loop index
	// Get length of string b
	n = strlen(b);
	// Checks the message header from the client
	for (i = 0; i <= n; i++) {
		if ((b[i] == 'R') && (b[i + 1] == 'E') && (b[i + 2] == 'G')) 
		{
			//First 3 chars are REG, corresponds to registering a username
			return 1;
		}
		else if((b[i] == 'U') && (b[i + 1] == 'N') && (b[i + 2] == 'R'))
		{
			//First 3 chars are UNR, corresponds to un-registering a username
			return 2;
		}
		else if((b[i] == 'Q') && (b[i + 1] == 'U') && (b[i + 2] == 'O'))
		{
			//First 3 chars are QUO, corresponds to asking for stock quotes
			return 3;
		}
	}
}