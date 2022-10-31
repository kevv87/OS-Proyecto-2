#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 2048

// set up global variables
int clientSocket = 0;
int answer;
int clientFd;
int closeFlag = 1;

struct sockaddr_in serverAddress;

char buffer[BUFFER_SIZE] = { 0 };

char *message = "Hello from client";

void *sendMessage(void *ptr) {

	char *message;

	message = (char*) ptr;

	send(clientSocket, message, strlen(message), 0);

	printf("Hello message sent\n");

	answer = read(clientSocket, buffer, BUFFER_SIZE);

	printf("%s\n", buffer);

	closeFlag = 0;

}

void *runClient(void *vargp) {

	if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {

		printf("\nSocket creation error \n");

		return 0;

	}

	serverAddress.sin_family = AF_INET;
	
	serverAddress.sin_port = htons(PORT);

	if(inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr) <= 0) {

		printf("\nInvalid address/ Address not supported \n");

		return 0;

	}

	if((clientFd = connect(clientSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress))) < 0) {

		printf("\nConnection Failed \n");

		return 0;

	}

	while(closeFlag) {

		// Keeping client running

	}

	close(clientFd);

}

void closeClient() {

	closeFlag = 0;

}

int main(int argc, char const* argv[]) {

	pthread_t clientThread;
	pthread_t sendThread;

	pthread_create(&clientThread, NULL, runClient, NULL);
	pthread_create(&sendThread, NULL, sendMessage, (void*) message);

	pthread_join(sendThread, NULL);
	pthread_join(clientThread, NULL);

	return 0;

}
