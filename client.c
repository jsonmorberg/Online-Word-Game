/* client.c - code for client. Do not rename this file */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>


int main( int argc, char **argv) {
	struct hostent *ptrh; /* pointer to a host table entry */
	struct protoent *ptrp; /* pointer to a protocol table entry */
	struct sockaddr_in sad; /* structure to hold an IP address */
	int sd; /* socket descriptor */
	int port; /* protocol port number */
	char *host; /* pointer to host name */
	char buf[1000]; /* buffer for data from the server */
	struct pollfd mypoll = { STDIN_FILENO, POLLIN|POLLPRI }; 

	memset((char *)&sad,0,sizeof(sad)); /* clear sockaddr structure */
	sad.sin_family = AF_INET; /* set family to Internet */


	if( argc != 3 ) {
		fprintf(stderr,"Error: Wrong number of arguments\n");
		fprintf(stderr,"usage:\n");
		fprintf(stderr,"./client server_address server_port\n");
		exit(EXIT_FAILURE);
	}

	port = atoi(argv[2]); /* convert to binary */
	if (port > 0) /* test for legal value */
		sad.sin_port = htons((u_short)port);
	else {
		fprintf(stderr,"Error: bad port number %s\n",argv[2]);
		exit(EXIT_FAILURE);
	}

	host = argv[1]; /* if host argument specified */

	/* Convert host name to equivalent IP address and copy to sad. */
	ptrh = gethostbyname(host);
	if ( ptrh == NULL ) {
		fprintf(stderr,"Error: Invalid host: %s\n", host);
		exit(EXIT_FAILURE);
	}

	memcpy(&sad.sin_addr, ptrh->h_addr, ptrh->h_length);

	/* Map TCP transport protocol name to protocol number. */
	if ( ((long int)(ptrp = getprotobyname("tcp"))) == 0) {
		fprintf(stderr, "Error: Cannot map \"tcp\" to protocol number");
		exit(EXIT_FAILURE);
	}

	/* Create a socket. */
	sd = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
	if (sd < 0) {
		fprintf(stderr, "Error: Socket creation failed\n");
		exit(EXIT_FAILURE);
	}


	/* Connect the socket to the specified server. */
	if (connect(sd, (struct sockaddr *)&sad, sizeof(sad)) < 0) {
		fprintf(stderr,"connect failed\n");
		exit(EXIT_FAILURE);
	}

	//Recieve Player #, Board Size, and Seconds per Round
	char player;
	recv(sd, &player, sizeof(char), 0);

	uint8_t boardSize;
	recv(sd,&boardSize,sizeof(uint8_t),0);

	uint8_t seconds;
	recv(sd,&seconds,sizeof(uint8_t),0);
	
	int timeout = seconds * 1000;

	//Print Game info
	if(player == '1'){
		printf("You are Player 1... the game will begin when Player 2 joins...\n");
	}else{
		printf("You are Player 2...\n");
	}

	printf("Board size: %d\n", boardSize);
	printf("Seconds per turn: %d\n", seconds);

	uint8_t p1Score;
	uint8_t p2Score;
	uint8_t round;
	char buffer[1000];

	//LOOP FOR ROUNDS
	for(;;){

		//Get scores, round # and board
		recv(sd, &p1Score, sizeof(uint8_t), 0);
		recv(sd, &p2Score, sizeof(uint8_t), 0);
		
		if(p1Score == 3 || p2Score == 3){
			break;
		}

		recv(sd, &round, sizeof(uint8_t), 0);
		recv(sd, buffer, boardSize, 0);

		//Print round info
		printf("\n");
		printf("Round is %d...\n", round);
		printf("Score is %d-%d\n", p1Score, p2Score);
		printf("Board: ");

		for(int i = 0; i < boardSize; i++){
			printf("%c ", buffer[i]);
		}
		printf("\n");

		//Receive turn
		char turn;
		recv(sd, &turn, sizeof(char), 0);

		//Check turn
		int turnFlag = 0;
		if(turn == 'Y'){
			turnFlag = 1;
		}

		//Gameplay Loop
		for(;;){
			char charbuf[1000];
			uint8_t wordSize;
    		uint8_t outcome;

			//Active
			if(turnFlag){
				
				//Use poll to add timeout to user input
				printf("Your turn, enter word: ");
				fflush(stdout);

				if(poll(&mypoll, 1, timeout))    { 

					scanf("%s", charbuf);  

					wordSize = strlen(charbuf);
					send(sd, &wordSize, sizeof(uint8_t), 0);
                	send(sd, charbuf, wordSize,0);
				} else { 
					printf("TIMED OUT\n");
					wordSize = 0;
					send(sd, &wordSize, sizeof(uint8_t), 0);
				} 

				//Recieve outcome of word validation
                recv(sd, &outcome, sizeof(uint8_t),0);
                if(outcome){
                    printf("Valid word!\n");
					turnFlag = 0;
                }else{
                    printf("Invalid Word\n");
                    break;
                }

			//Inactive
			}else{
				
				//Wait for server's response
				printf("Please wait for opponent to enter word ...\n");
                recv(sd,&wordSize, sizeof(uint8_t),0);
                if(wordSize == 0){
                    printf("Opponent lost the round!\n");
                    break;
                }else{
                    recv(sd, buf, wordSize, 0);
					buf[wordSize] = '\0';
					printf("Opponent entered: %s\n", buf);
					turnFlag = 1;
				}
			}
		}
	
	}

	//Print result
	if(player == '1'){
		if(p1Score == 3){
			printf("You won!\n");
		}else{
			printf("You lost!\n");
		}
	}else{
		if(p2Score == 3){
			printf("You won!\n");
		}else{
			printf("You lost!\n");
		}
	}
	close(sd);
}