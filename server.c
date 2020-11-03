/* server.c - code for server. Do not rename this file */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "trie.h"

#define QLEN 6 /* size of request queue */ 
trieNode *dictionary;

//This method checks to see if a word is valid
int validWord(trieNode * usedWords, char* word, char* board){

	//copy the board
	char newBoard[sizeof(board)];
	size_t destination_size = sizeof(board);
	strncpy(newBoard, board, destination_size);
	newBoard[destination_size] = '\0';

	//Check if word is in dictionary
    if(!trieSearch(dictionary, word)){
		return 0;
	}
	
	//Check if word has been used before in round
	if(trieSearch(usedWords, word)){
		return 0;
	}

	//Check if individual characters are in board
	for(int i = 0; i < strlen(word); i++){
		char wordChar = word[i];
		int flag = 0;

		for(int j = 0; j < strlen(newBoard); j++){
            if(wordChar == newBoard[j]){
                flag = 1;
                newBoard[j] = ' ';
                break;
            }
        }

        if(flag == 0){
            return 0;
        }
	}

	//Word is valid
    return 1;
}

//Generates a board of random characters
char* generateBoard(int boardSize){

	//Seed the random function
    srand(time(NULL));

	//Store alphabet and vowels for board construction
    char abc[26]="abcdefghijklmnopqrstuvwxyz";
    char vowel[5]="aeiou";
    char *board;
    board = malloc(boardSize);

	//Generate random chars for board
    for(int i = 0; i < boardSize; i++){
        board[i] = abc[rand() % (sizeof(abc) - 1)];
    }

	//Check if there was any vowels
    int vowelFlag = 0;
    for(int i = 0; i < boardSize; i++){
        if(board[i] == 'a' || board[i] == 'e'|| board[i] == 'i'|| board[i] == 'o'|| board[i] == 'u'){
            vowelFlag = 1;
        }
    }


	//If there is no vowel, append vowel in end of board
    if(!vowelFlag){
        board[boardSize-1] = vowel[rand() % (sizeof(vowel) - 1)];
    }

    return board;
}

//Game function
void game(int p1, int p2, int boardSize){

	//Variables to store socket info and game info
	int active;
	int inactive;
	char first = 'Y';
	char second = 'N';

	uint8_t round = 1;
	uint8_t p1Score = 0;
    uint8_t p2Score = 0;

	//Loop till one player reaches 3
	while(p1Score != 3 && p2Score != 3){

		//Send the scores to both players
		send(p1, &p1Score, sizeof(uint8_t), 0);
		send(p1, &p2Score, sizeof(uint8_t), 0);
		send(p2, &p1Score, sizeof(uint8_t), 0);
		send(p2, &p2Score, sizeof(uint8_t), 0);

		//Send the round
		send(p1, &round, sizeof(uint8_t), 0);
		send(p2, &round, sizeof(uint8_t), 0);

		//Generate Board and send it
		char* board = generateBoard(boardSize);
		send(p1, board, boardSize, 0);
		send(p2, board, boardSize, 0);

		//Figure out which player goes first
		if(round % 2 == 0){
			active = p2;
			inactive = p1;
		}else{
			active = p1;
			inactive = p2;
		}

		//Create trie stucture to store used words
		trieNode *usedWords = trieCreate();

		//Send Y and N to correct clients
		send(active, &first, sizeof(char), 0);
		send(inactive, &second, sizeof(char), 0);

		//TURNS
		for(;;){
			
			//Get size of guessed word
			uint8_t wordSize;
			char buf[1000];

			//Check if active player has disconnected
			int connection = recv(active, &wordSize, sizeof(uint8_t), 0);

			//Client has disconnected, server closes network connection and ends process
			if(connection == 0){
				close(active);
				close(inactive);
				printf("Client disconnected from session, game has ended for that session")
				exit(EXIT_SUCCESS);
			}

			//Client did not timeout
			if(wordSize != 0){

				//Get word
				recv(active, buf, wordSize, 0);
				buf[wordSize] = '\0';
			}

			if(validWord(usedWords, buf, board)){

				//Valid word gets put in Trie
				trieInsert(usedWords, buf);

				//Send 1 to active player
				uint8_t one = 1;
				send(active, &one, sizeof(uint8_t), 0);

				//Send word size and word to inactive player
				send(inactive, &wordSize, sizeof(uint8_t), MSG_NOSIGNAL);
				send(inactive, buf, wordSize, MSG_NOSIGNAL);
			}else{
				
				//Send Zeros to both players
				uint8_t zero = 0;
				send(active, &zero, sizeof(uint8_t), 0);
				send(inactive, &zero, sizeof(uint8_t), 0);

				//Increase score of inactive player
				if(inactive == p1){
					p1Score+=1;
				}else{
					p2Score+=1;
				}

				//turn is over
				break;
			}

			//Swap active and inactive player
			if(active == p1){
				active = p2;
				inactive = p1;
			}else{
				active = p1;
				inactive = p2;
			}
		}
		//Increment round #
		round++;
		free(board);
		free(usedWords);
	}

	//Send the scores to both players
	send(p1, &p1Score, sizeof(uint8_t), 0);
	send(p1, &p2Score, sizeof(uint8_t), 0);
	send(p2, &p1Score, sizeof(uint8_t), 0);
	send(p2, &p2Score, sizeof(uint8_t), 0);

}

int main(int argc, char **argv) {
	struct protoent *ptrp; /* pointer to a protocol table entry */
	struct sockaddr_in sad; /* structure to hold server's address */
	struct sockaddr_in cad1; /* structure to hold client's address */
    struct sockaddr_in cad2; /* structure to hold client's address */
	int sd, sd2, sd3; /* socket descriptors */
	int port; /* protocol port number */
	socklen_t alen; /* length of address */
	int optval = 1; /* boolean value when we set socket option */
	struct timeval tv; /* structure to manipulate timeout for socket*/

	//Args
    uint8_t boardSize;
    uint8_t seconds;
    char * wordPath;

	//Arg checking
	if( argc != 5 ) {
		fprintf(stderr,"Error: Wrong number of arguments\n");
		fprintf(stderr,"usage:\n");
		fprintf(stderr,"./server server_port board_size seconds path_to_dictionary\n");
		exit(EXIT_FAILURE);
	}

	wordPath = argv[4];
    seconds = atoi(argv[3]);
	if(seconds < 0){
		fprintf(stderr,"Error: Seconds can't be negative\n");
		exit(EXIT_FAILURE);
	}
	tv.tv_sec = seconds;
	tv.tv_usec = 0;

    boardSize = atoi(argv[2]);
	if(boardSize< 0){
		fprintf(stderr,"Error: Board size can't be negative\n");
		exit(EXIT_FAILURE);
	}

	//Create and load dictionary
	dictionary = trieCreate(); 

    FILE* filePointer;
    int bufferLength = 1000;
    char buffer[bufferLength];

    filePointer = fopen(wordPath, "r");

    while(fgets(buffer, bufferLength, filePointer)) {
		buffer[strlen(buffer)-1] = 0;
        trieInsert(dictionary, buffer);
    }

    fclose(filePointer);

	memset((char *)&sad,0,sizeof(sad)); /* clear sockaddr structure */
	sad.sin_family = AF_INET; /* set family to Internet */
	sad.sin_addr.s_addr = INADDR_ANY; /* set the local IP address */
    
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

	pid_t pid;
	while (1) {

		//Player 1 Connect
		alen = sizeof(cad1);
		if ((sd2=accept(sd, (struct sockaddr *)&cad1, &alen)) < 0) {
			fprintf(stderr, "Error: Accept fuiled\n");
			exit(EXIT_FAILURE);
		}

		//Send Player #, Board Size, and Seconds per Round
		char playerType = '1';
        send(sd2,&playerType,sizeof(char),0);
        send(sd2,&boardSize,sizeof(uint8_t),0);
        send(sd2,&seconds,sizeof(uint8_t),0);

		//Player 2 Connect
        alen = sizeof(cad2);
		if ((sd3=accept(sd, (struct sockaddr *)&cad2, &alen)) < 0) {
			fprintf(stderr, "Error: Accept filed\n");
			exit(EXIT_FAILURE);
		}

		//Send Player #, Board Size, and Seconds per Round
		playerType = '2';
        send(sd3,&playerType,sizeof(char),0);
        send(sd3,&boardSize,sizeof(uint8_t),0);
        send(sd3,&seconds,sizeof(uint8_t),0);

		/* Allow reuse of port - avoid "Bind failed" issues */
		if( setsockopt(sd2, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)) < 0 ) {
			fprintf(stderr, "Error Setting socket option failed\n");
			exit(EXIT_FAILURE);
		}

		/* Allow reuse of port - avoid "Bind failed" issues */
		if( setsockopt(sd3, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)) < 0 ) {
			fprintf(stderr, "Error Setting socket option failed\n");
			exit(EXIT_FAILURE);
		}
		
		pid = fork();

		if(pid < 0){
			fprintf(stderr, "Error: Fork error\n"); 
            exit(EXIT_FAILURE); 
		}

		//Child Process
		if(pid == 0){

			close(sd);
			game(sd2, sd3, boardSize);
			close(sd2);
			close(sd3);
			exit(EXIT_SUCCESS);
		}
	}
}



