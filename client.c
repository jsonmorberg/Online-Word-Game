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

int main( int argc, char **argv) {
	struct hostent *ptrh; /* pointer to a host table entry */
	struct protoent *ptrp; /* pointer to a protocol table entry */
	struct sockaddr_in sad; /* structure to hold an IP address */
	int sd; /* socket descriptor */
	int port; /* protocol port number */
	char *host; /* pointer to host name */
	char buf[1000]; /* buffer for data from the server */

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

	char player;
	recv(sd, &player, sizeof(char), 0);

	uint8_t boardSize;
	recv(sd,&boardSize,sizeof(boardSize),0);

	uint8_t seconds;
	recv(sd,&seconds,sizeof(seconds),0);

	if(player == '1'){
		printf("You are Player 1... the game will begin when Player 2 joins...\n");
	}else{
		printf("You are Player 2...\n");
	}

	printf("Board size: %d\n", boardSize);
	printf("Seconds per turn: %d\n", seconds);

	uint8_t playerScore1;
	uint8_t playerScore2;
	uint8_t round;
	char buffer[1000];

	for(;;){
		recv(sd, &playerScore1, sizeof(playerScore1), 0);
		recv(sd, &playerScore2, sizeof(playerScore2), 0);
		recv(sd, &round, sizeof(round), 0);
		recv(sd, buffer, boardSize, 0);

		printf("Round is %d...\n", round);
		printf("Score is %d-%d\n", playerScore1, playerScore2);
		printf("Board: ");

		for(int i = 0; i < boardSize; i++){
			printf("%c ", buffer[i]);
		}
		printf("\n");

		char turn;
		recv(sd, &turn, sizeof(char), 0);

		int turnFlag = 0;

		if(turn == 'Y'){
			turnFlag = 1;
		}

		for(;;){
			char charbuf[1000];
			uint8_t guessSize;
    		uint8_t outcome;

			if(turnFlag){

				printf("Your turn, enter word: ");
                scanf("%s", charbuf);

                guessSize = strlen(charbuf);

                send(sd, &guessSize, sizeof(uint8_t), 0);
                send(sd, &charbuf, guessSize,0);

                recv(sd, &outcome, sizeof(uint8_t),0);
                if(outcome){
                    printf("Valid word!\n");
					turnFlag = 0;
                }else{
                    printf("Invalid Word\n");
                    break;
                }

			}else{

				printf("Please wait for opponent to enter word ...\n");
                recv(sd,&guessSize, sizeof(uint8_t),0);
                if(guessSize == 0){
                    printf("Opponent lost the round!\n");
                    break;
                }else{
                    recv(sd, buf, guessSize, 0);
                    buf[guessSize] = '\0';
					printf("Opponent entered: %s", buf);
					turnFlag = 1;
				}
			}
		}
	
	}
}

