#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>


#define PORT 8080

#define END "end"


// set up global variables
int clientSocket = 0;
int answer;
int clientFd;
int close = 1;

struct sockaddr_in serverAddress;

//char* message = "Hello from client";
char buffer[2048] = { 0 };






void *sendMessage(void *ptr) {

	char *message;

	message = (char*) ptr;

	send(clientSocket, message, strlen(message), 0);

	printf("Hello message sent\n");

	answer = read(clientSocket, buffer, 2048);

	printf("%s\n", buffer);

	close = 0;

}

void *runClient(void *vargp) {

	if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {

		printf("\nSocket creation error \n");

		return -1;

	}

	serverAddress.sin_family = AF_INET;
	
	serverAddress.sin_port = htons(PORT);

	if(inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr) <= 0) {

		printf("\nInvalid address/ Address not supported \n");

		return -1;

	}

	if((clientFd = connect(clientSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress))) < 0) {

		printf("\nConnection Failed \n");

		return -1;

	}

	while(close) {

		// Keeping client running

	}

	close(clientFd);

}

void closeClient() {

	close = 0;

}





















int main(int argc, char const* argv[]) {

	/*
	int clientSocket = 0, answer, clientFd;

	struct sockaddr_in serverAddress;

	char* message = "Hello from client";

	char buffer[1024] = { 0 };
	

	
	if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {

		printf("\nSocket creation error \n");

		return -1;
	}

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(PORT);

	if(inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr) <= 0) {

		printf("\nInvalid address/ Address not supported \n");

		return -1;

	}

	if((clientFd = connect(clientSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress))) < 0) {

		printf("\nConnection Failed \n");

		return -1;

	}
	

    char* end = "end";

    
	
    while(strcmp(message, end)) {

        send(clientSocket, message, strlen(message), 0);
        send(clientSocket, message, strlen(message), 0);
        send(clientSocket, message, strlen(message), 0);

        printf("Hello message sent\n");

        answer = read(clientSocket, buffer, 1024);

        printf("%s\n", buffer);

        message = "end";

    }
	
	close(clientFd);
	*/

	pthread_t clientThread;

	pthread_create(&clientThread, NULL, runClient, NULL);


	char *message = "Hello from client";

	pthread_t sendThread;

	pthread_create(&sendThread, NULL, sendMessage, (void*) message);









	pthread_join(sendThread, NULL);
	pthread_join(clientThread, NULL);








	return 0;

}
