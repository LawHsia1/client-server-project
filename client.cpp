#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXLINE 4096 /*max text line length*/
#define SERV_PORT 8000 /*port*/
#define MATRIXSIZE 3

int matrixA[MATRIXSIZE][MATRIXSIZE];
int matrixB[MATRIXSIZE][MATRIXSIZE];
int resultMatrix[MATRIXSIZE][MATRIXSIZE];

void initialize()
{
	// initializes matrixA and matrixB to identity matrices
	for(int i = 0; i < MATRIXSIZE; i++)
	{
		for(int j = 0; j < MATRIXSIZE; j++)
		{
			//initialize diagnanols to 1
			if(i == j)
			{
				matrixA[i][j] = 2;
				matrixB[i][j] = 3;
			}
			else
			{
				matrixA[i][j] = 1;
				matrixB[i][j] = 1;
			}
		}
	}
	/*
	//ray wutoi;asdfj;asdfjklasdfio;aefwew;
	matrices = (Matrices*) malloc(sizeof(Matrices));
	
	memcpy(matrices->A, A, MATRIXSIZE * MATRIXSIZE * sizeof(int));
	memcpy(matrices->B, B, MATRIXSIZE * MATRIXSIZE * sizeof(int));
	*/
}

void printMatrix(int matrix[][MATRIXSIZE], int columns, int rows)
{
	for(int i = 0; i < columns; i++)
	{
		for(int j = 0; j < rows; j++)
		{
			printf("%d\t", matrix[i][j]);
		}
		
		printf("\n");
	}
}


int main(int argc, char **argv)
{
	int sockfd;
	int portnum;
	struct sockaddr_in servaddr = {};
	
	//basic check of the arguments
	if (argc !=2) {
		perror("Usage: TCPClient <IP address of the server");
		exit(1);
	}
	
	initialize();
	
	//Create a socket for the client
	//If sockfd<0 there was an error in the creation of the socket
	if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Problem in creating the socket");
		exit(2);
	}
	
	// fill out info about server addr that is to be connected to
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr= inet_addr(argv[1]);
	servaddr.sin_port =  htons(SERV_PORT); //convert to big-endian order
	
	//attempt to connect to the server
	if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
		perror("Problem in connecting to the server");
		exit(3);
	}

	printf("Matrix A is \n");
	printMatrix(matrixA, MATRIXSIZE, MATRIXSIZE);
	printf("\n");
	
	printf("Matrix B is \n");
	printMatrix(matrixB, MATRIXSIZE, MATRIXSIZE);
	printf("\n");
	
	// send matrix A
	write(sockfd, matrixA, sizeof(matrixA));
	// send matrix B
	write(sockfd, matrixB, sizeof(matrixB));
	
	
	// receive the result
	read(sockfd, resultMatrix, sizeof(resultMatrix));
	
	printf("A x B is \n");
	printMatrix(resultMatrix, MATRIXSIZE, MATRIXSIZE);
	printf("\n");
	
	return 0;
}
