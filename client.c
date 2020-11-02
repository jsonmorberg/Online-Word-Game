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

int validWord(trieNode* usedWords, char* word, char* board){
	char newBoard[sizeof(board)];
	size_t destination_size = sizeof(board);

	strncpy(newBoard, board, destination_size);
	newBoard[destination_size] = '\0';

    if(!trieSearch(dictionary, word)){
		return 1;
	}

	if(trieSearch(usedWords, word)){
		return 1;
	}

	for(int i = 0; i < strlen(word); i++){
		char wordChar = word[i];
		int flag = 0;

		for(int j = 0; j < strlen(newBoard); j++){
            if(wordChar == newBoard[j]){
                flag = 1;
                newBoard[j] = ' ';
                printf("%s\n", newBoard);
                break;
            }
        }

        if(flag == 0){
            return 0;
        }
	}

    return 1;
}

int main( int argc, char **argv) {
	struct hostent *ptrh; /* pointer to a host table entry */
	struct protoent *ptrp; /* pointer to a protocol table entry */
	struct sockaddr_in sad; /* structure to hold an IP address */
	int sd; /* socket descriptor */
	int port; /* protocol port number */
	char *host; /* pointer to host name */
	int n; /* number of characters read */
	char buf[1000]; /* buffer for data from the server */
    uint8_t boardSize;
    uint8_t seconds;
    uint8_t player;
    uint8_t round;
    uint8_t score;
    uint8_t opScore;
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
    printf("%d, %d, %d\n", player,  boardSize, seconds);
    score = 69;
    //GAME START
    recv(sd, &round, sizeof(uint8_t), 0);
    printf("starting round %d\n", round);
    send(sd, &score, sizeof(uint8_t), 0);
    recv(sd, &opScore, sizeof(uint8_t), 0);
    printf("my = %d op = %d\n",score, opScore);
    char board[boardSize];
    recv(sd, board, boardSize,0);
    board[boardSize] = '\0';
    printf("%s\n", board);
    //recive board
	close(sd);
	exit(EXIT_SUCCESS);
}

