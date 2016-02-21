#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <unistd.h>

#define SERV_PORT 8000 /*port*/
#define LISTENQ 5 /*maximum number of client connections */
#define BUFFSIZE 10
#define MATRIXSIZE 3


//global variables
// one thread calculates one column
// thus, # threads = # columns
pthread_t multipliers[MATRIXSIZE]; 

int matrixA[MATRIXSIZE][MATRIXSIZE];
int matrixB[MATRIXSIZE][MATRIXSIZE];
int matrixC[MATRIXSIZE][MATRIXSIZE];

void error(char *msg, int errorNum)
{
	printf("Unable to ");
}

//gets the column of what number is plugged in
//still have to account for that 0th index and stuff
// the work of a single thread
void *multiply(void * cWant)
{
	int col;
	int row;
	int columnWanted = (int)cWant; //cWant aka columnWanted indiciates which columns are to be multiplied
	
	for(col = 0; col < MATRIXSIZE; col++)
	{
		for(row = 0; row < MATRIXSIZE; row++)
		{
			matrixC[col][columnWanted] += matrixA[columnWanted][row] * matrixB[row][col];
		}
	}
}

void clearToZero(int matrix[MATRIXSIZE][MATRIXSIZE])
{
	for(int i = 0; i < MATRIXSIZE; i++)
	{
		for(int j = 0; j < MATRIXSIZE; j++)
		{
			matrix[i][j] = 0;
		}
	}
}

//has all the threads run
//gets A x B
void processData()
{
	int i;
	for(i = 0; i < MATRIXSIZE; i++)//run all threads
		pthread_create(&multipliers[i], NULL, multiply, (void *)i );
	
	for(i = 0; i < MATRIXSIZE; i++)//join all threads
		pthread_join(multipliers[i], NULL);	
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

int main (int argc, const char * argv[]) 
{	
	int listenfd, connfd, n;
	socklen_t clilen;
	char buf[BUFFSIZE];
	struct sockaddr_in cliaddr, servaddr;	
	
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	
	//preparing the socket address
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
	
	bind (listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	
	listen (listenfd, LISTENQ);
		
	printf("%s\n","Matrix Multiplier Server running...waiting for connections.");
	printf("Close the terminal window to exit the server at any time.\n");
	//processData();
	while( true ) { // if server reads in EOF, fgets returns null and program terminates
		clilen = sizeof(cliaddr);
		connfd = accept (listenfd, (struct sockaddr *) &cliaddr, &clilen); //connfd = file descriptor to socket that sends data to client
		printf("%s\n","Received request...");		
		
		n = read(connfd, matrixA, sizeof(matrixA));
		if(n < 0)
		{
			printf("Unable to read data from socket");
			exit(2);
		}
		else
		{
			printf("Received matrix A\n");
			printMatrix(matrixA, MATRIXSIZE, MATRIXSIZE);
		}
		
		n = read(connfd, matrixB, sizeof(matrixB));		
		if(n < 0)
		{
			printf("Unable to read data from socket");
			exit(2);
		}
		else
		{
			printf("Received matrix B\n");
			printMatrix(matrixB, MATRIXSIZE, MATRIXSIZE);
		}
		
		processData(); // this will calculate the result matrix, aka matrixC
		write(connfd, matrixC, sizeof(matrixC)); // send matrixC back to client
		// clear out matrixC since it will be used again for another calculation when another client connects
		clearToZero(matrixC);
		close(connfd);
	}
	//close listening socket
	close (listenfd);
    return 0;
}
