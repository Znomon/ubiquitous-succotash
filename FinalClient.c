//================================================= file = catdogClient.c =====
//= A UDP client to send strings to catdogServer (with retry capability) =
//=============================================================================
//= Notes: =
//= 1) This program conditionally compiles for Winsock and BSD sockets. =
//= Set the initial #define to WIN or BSD as appropriate. =
//= 2) This program assumes that the IP address of the host running =
//= udpServer is defined in "#define IP_ADDR" =
//= 3) The time-out is the #define as MAX_WAIT =
//=---------------------------------------------------------------------------=
//= Example execution: =
//=---------------------------------------------------------------------------=
//= Build: bcc32 catdogClient.c or cl catdogClient.c wsock32.lib for Winsock =
//= gcc catdogClient.c -lsocket -lnsl for BSD =
//=---------------------------------------------------------------------------=
//= Execute: catdogClient =
//=---------------------------------------------------------------------------=
//= Author: Ken Christensen =
//= University of South Florida =
//= WWW: http://www.csee.usf.edu/~christen =
//= Email: christen@csee.usf.edu =
//=---------------------------------------------------------------------------=
//= History: KJC (09/11/15) - Genesis (from udpClient.c) =
//=============================================================================
#define BSD // WIN for Winsock and BSD for BSD sockets
//----- Include files ---------------------------------------------------------
#include <stdio.h> // Needed for printf()
#include <string.h> // Needed for memcpy() and strcpy()
#include <stdlib.h> // Needed for exit()
#include <time.h> // Needed for time()
#ifdef WIN
#include < windows.h > // Needed for all Winsock stuff
#endif
#ifdef BSD
#include <sys/types.h> // Needed for sockets stuff
#include <netinet/in.h> // Needed for sockets stuff
#include <sys/socket.h> // Needed for sockets stuff
#include <arpa/inet.h> // Needed for sockets stuff
#include <fcntl.h> // Needed for sockets stuff
#include <netdb.h> // Needed for sockets stuff
#endif
//----- Defines ---------------------------------------------------------------
#define PORT_NUM 1050 // Port number used
#define IP_ADDR "127.0.0.1" //"66.158.200.153" // IP address of server1 (*** HARDWIRED ***)
#define MAX_WAIT 5 // Maximum wait in seconds
//===== Main program ==========================================================
int clean_stdin()
{
    while (getchar()!='\n');
    return 1;
}

int main(int argc, char * argv[]) {
	#ifdef WIN
	WORD wVersionRequested = MAKEWORD(1, 1); // Stuff for WSA functions
	WSADATA wsaData; // Stuff for WSA functions
	#endif
	int client_s; // Client socket descriptor
	struct sockaddr_in server_addr; // Server Internet address
	unsigned long int noBlock; // Non-blocking flag
	int addr_len; // Internet address length
	char out_buf[4096]; // Output buffer for data
	char in_buf[4096]; // Input buffer for data
	int retcode; // Return code
	int timeStart; // Time of start for loop
	int i; // Loop counter

	// Output usage
	/* if (argc != 2)
 {
 printf("Usage: catdogClient <string> where <string> is a maximum 140 \n");
 printf(" character string to be cat/dog converted and reflected \n");
 exit(1);
 } */
#ifdef WIN
	// This stuff initializes winsock
	WSAStartup(wVersionRequested, & wsaData);
	#endif
	// Create a socket
	client_s = socket(AF_INET, SOCK_DGRAM, 0);
	if (client_s < 0) {
		printf("*** ERROR - socket() failed \n");
		exit(-1);
	}
	#ifdef WIN
	// Make client_s non-blocking
	noBlock = 1;
	ioctlsocket(client_s, FIONBIO, & noBlock);
	#endif
	#ifdef BSD
	int flags = fcntl(client_s, F_GETFL, 0);
	fcntl(client_s, F_SETFL, flags | O_NONBLOCK);
	#endif
	// Fill-in server1 socket's address information
	server_addr.sin_family = AF_INET; // Address family to use
	server_addr.sin_port = htons(PORT_NUM); // Port num to use
	server_addr.sin_addr.s_addr = inet_addr(IP_ADDR); // IP address to use
	// Loop until receive reply message
	timeStart = time(NULL);

	char username[] = "null";

	while (1) {
		while(1)
		{
			int input = -1;
			char c;
			do
			{
			printf("Select your operation. \n1. Register Username\n2. Un-register Username\n3. Request Quote(s)\nYour Selection:");
			} while (((scanf("%d%c", &input, &c)!=2 || c!='\n') && clean_stdin()) || input<1 || input>3);
			switch(input)
			{
				case 1:
					printf("\nPlease enter a username to register with:");
					scanf("%s", &username);
					// TODO : if username fits criteria send, else loop
					sprintf(out_buf, "REG,%s;", username);
					goto sendPkt;
					break; //should never reach here
				case 2:
					printf("\nPlease enter a username you want to un-register:");
					scanf("%s", &username);
					sprintf(out_buf, "UNR,%s;", username);
					goto sendPkt;
					break;
				case 3:
					printf("\n user requested to see stock quotes \n");
					break;
			}
		}
		sendPkt:

		
		// Assign a message to buffer out_buf
		//strcpy(out_buf, argv[1]);
		// Now send the message to server.
		retcode = sendto(client_s, out_buf, (strlen(out_buf) + 1), 0, (struct sockaddr * ) & server_addr, sizeof(server_addr));
		if (retcode < 0) {
			printf("*** ERROR - sendto() failed \n");
			exit(-1);
		}
		//printf("Send to server: %s \n", out_buf);
		// Wait to receive a message
		retcode = 0;
		for (i = 0; i < MAX_WAIT; i++) {
			addr_len = sizeof(server_addr);
			retcode = recvfrom(client_s, in_buf, sizeof(in_buf), 0, (struct sockaddr * ) & server_addr, & addr_len);
			if (retcode > 0) {
				printf("Elapsed time = %d seconds \n", time(NULL) - timeStart);
				printf("Received from server: %s \n", in_buf);

			}
			#ifdef WIN
			Sleep(1000); // Windows Sleep for 1000 ms (1 second)
			#endif
			#ifdef BSD
			sleep(1); // Unix sleep for 1 second
			#endif
		}
	}
	exit:
	// Close all open sockets
	#ifdef WIN
	retcode = closesocket(client_s);
	if (retcode < 0) {
		printf("*** ERROR - closesocket() failed \n");
		exit(-1);
	}
	#endif
	#ifdef BSD
	retcode = close(client_s);
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