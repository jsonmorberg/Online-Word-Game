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
	int n; /* number of characters read */
	char buf[1000]; /* buffer for data from the server */
    char charbuf[1000];
    uint8_t boardSize;
    uint8_t seconds;
    uint8_t player;
    uint8_t round;
    uint8_t score;
    uint8_t opScore;
    uint8_t guessSize;
    uint8_t outcome;
    char active[1];
    char opguess[1000];
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
    recv(sd,&player,sizeof(uint8_t),0);
    recv(sd,&boardSize,sizeof(uint8_t),0);
    recv(sd,&seconds,sizeof(uint8_t),0);
    printf("You are Player %d...\n",player);
    printf("Board Size: %d\n", boardSize);
    printf("Seconds per turn: %d\n", seconds);
    score = 0;
    //GAME START
    while(1){
        printf("\n");
        recv(sd, &round, sizeof(uint8_t), 0);
        if(round != 255){
            printf("starting round %d\n", round);
        }
        recv(sd, &opScore, sizeof(uint8_t), 0);
        recv(sd, &score, sizeof(uint8_t), 0);
        if(round == 255){
            break;
        }
        printf("score is %d-%d\n",opScore, score);
        char board[boardSize];
        //RECV BOARD
        recv(sd, board, boardSize,0);
        board[boardSize] = '\0';
        printf("Board: %s\n", board);
        //START TURN
        //recive whos active
        while(1){
            recv(sd, active, sizeof('Y'), 0);
            if(active[0] == 'Y'){
                printf("Your turn, enter word: ");
                scanf("%s", charbuf);
                guessSize = strlen(charbuf);
                send(sd, &guessSize, sizeof(uint8_t), 0);
                send(sd, &charbuf, guessSize,0);
                recv(sd, &outcome, sizeof(uint8_t),0);
                if(outcome){
                    printf("Valid word!\n");
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
                    recv(sd, opguess, guessSize, 0);
                    opguess[guessSize] = '\0';
                    printf("Opponent entered  %s\n", opguess);

                }                                              
            }
        }
    }//end of while
    if(opScore < score){
        printf("You lost!\n");
    }else{
        printf("You won!\n");
    }
	close(sd);
	exit(EXIT_SUCCESS);
}

