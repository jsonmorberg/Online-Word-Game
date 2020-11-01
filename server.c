/* server.c - code for server. Do not rename this file */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define QLEN 6 /* size of request queue */
int visits = 0; /* counts client connections */

int main(int argc, char **argv) {
	struct protoent *ptrp; /* pointer to a protocol table entry */
	struct sockaddr_in sad; /* structure to hold server's address */
	struct sockaddr_in cad1; /* structure to hold client's address */
    struct sockaddr_in cad2; /* structure to hold client's address */
	int sd, sd2, sd3; /* socket descriptors */
	int port; /* protocol port number */
	int alen; /* length of address */
	int optval = 1; /* boolean value when we set socket option */
	char buf[1000]; /* buffer for string the server sends */
    uint8_t boardSize;
    uint8_t seconds;
    char * wordPath;
    uint8_t round;
    uint8_t p1Score;
    uint8_t p2Score;
	if( argc != 5 ) {
		fprintf(stderr,"Error: Wrong number of arguments\n");
		fprintf(stderr,"usage:\n");
		fprintf(stderr,"./server server_port\n");
		exit(EXIT_FAILURE);
	}

	memset((char *)&sad,0,sizeof(sad)); /* clear sockaddr structure */
	sad.sin_family = AF_INET; /* set family to Internet */
	sad.sin_addr.s_addr = INADDR_ANY; /* set the local IP address */
    
    wordPath = argv[4];
    seconds = atoi(argv[3]);
    boardSize = atoi(argv[2]);
	port = atoi(argv[1]); /* convert argument to binary */
	if (port > 0) { /* test for illegal value */
		sad.sin_port = htons((u_short)port);
	} else { /* print error message and exit */
		fprintf(stderr,"Error: Bad port number %s\n",argv[1]);
		exit(EXIT_FAILURE);
	}

	/* Map TCP transport protocol name to protocol number */
	if ( ((long int)(ptrp = getprotobyname("tcp"))) == 0) {
		fprintf(stderr, "Error: Cannot map \"tcp\" to protocol number");
		exit(EXIT_FAILURE);
	}

	/* Create a socket */
	sd = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
	if (sd < 0) {
		fprintf(stderr, "Error: Socket creation failed\n");
		exit(EXIT_FAILURE);
	}


	/* Allow reuse of port - avoid "Bind failed" issues */
	if( setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0 ) {
		fprintf(stderr, "Error Setting socket option failed\n");
		exit(EXIT_FAILURE);
	}

	/* Bind a local address to the socket */
	if (bind(sd, (struct sockaddr *)&sad, sizeof(sad)) < 0) {
		fprintf(stderr,"Error: Bind failed\n");
		exit(EXIT_FAILURE);
	}

	/* Specify size of request queue */
	if (listen(sd, QLEN) < 0) {
		fprintf(stderr,"Error: Listen failed\n");
		exit(EXIT_FAILURE);
	}
	while (1) {
		alen = sizeof(cad1);
		if ( (sd2=accept(sd, (struct sockaddr *)&cad1, &alen)) < 0) {
			fprintf(stderr, "Error: Accept failed\n");
			exit(EXIT_FAILURE);
		}
        uint8_t player = 1;
        send(sd2,&player,sizeof(uint8_t),0);
        send(sd2,&boardSize,sizeof(uint8_t),0);
        send(sd2,&seconds,sizeof(uint8_t),0);
        //send player 1
        //send boardnum
        //send secs
        alen = sizeof(cad2);
		if ( (sd3=accept(sd, (struct sockaddr *)&cad2, &alen)) < 0) {
			fprintf(stderr, "Error: Accept failed\n");
			exit(EXIT_FAILURE);
		}
        player = 2;
        send(sd3,&player,sizeof(uint8_t),0);
        send(sd3,&boardSize,sizeof(uint8_t),0);
        send(sd3,&seconds,sizeof(uint8_t),0);
        round = 3;
        //GAME START
        send(sd2, &round, sizeof(uint8_t), 0);
        send(sd3, &round, sizeof(uint8_t), 0);
            
        recv(sd2, &p1Score, sizeof(uint8_t),0);
        recv(sd3, &p2Score, sizeof(uint8_t), 0);
        send(sd2, &p2Score, sizeof(uint8_t), 0);
        send(sd3, &p1Score, sizeof(uint8_t), 0);
        //send board
        char board[boardSize];
        for(int i = 0; i < boardSize; i++){
            board[i] = 'a';
        }
        send(sd2, &board, sizeof(board), 0);
        send(sd3, &board, sizeof(board), 0);
        
        close(sd2);
		close(sd3);
	}
}

