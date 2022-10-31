#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define PORT 17017
#define BUFFER_SIZE 2048

// set up global variables
int clientSocket = 0;
int answer;
int clientFd;
int closeFlag = 1;

struct sockaddr_in serverAddress;

char buffer[BUFFER_SIZE] = { 0 };

char *message1 = "changeDirection";

char *message2 = "incrementLeft";
char *message3 = "decrementLeft";
char *message4 = "incrementRight";
char *message5 = "decrementRight";

char *message6 = "addBoat,2,2,0,0";

char *message7 = "removeBoat,0";

char *message8 = "moveBoat,0";

sem_t messageSemaphore;

void *sendMessage(void *ptr) {

	sem_wait(&messageSemaphore);

	char *message;

	message = (char*) ptr;

	send(clientSocket, message, strlen(message), 0);

	printf("Message sent\n");

	answer = read(clientSocket, buffer, BUFFER_SIZE);

	printf("%s\n", buffer);

	closeFlag = 0;

	sem_post(&messageSemaphore);

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

	sem_init(&messageSemaphore, 0, 1);

	pthread_t clientThread;

	//pthread_t sendThread1;
	//pthread_t sendThread2;
	//pthread_t sendThread3;
	//pthread_t sendThread4;
	//pthread_t sendThread5;
	//pthread_t sendThread6;
	//pthread_t sendThread7;
	pthread_t sendThread8;

	pthread_create(&clientThread, NULL, runClient, NULL);

	//pthread_create(&sendThread1, NULL, sendMessage, (void*) message1);
	//pthread_create(&sendThread2, NULL, sendMessage, (void*) message2);
	//pthread_create(&sendThread3, NULL, sendMessage, (void*) message3);
	//pthread_create(&sendThread4, NULL, sendMessage, (void*) message4);
	//pthread_create(&sendThread5, NULL, sendMessage, (void*) message5);
	//pthread_create(&sendThread6, NULL, sendMessage, (void*) message6);
	//pthread_create(&sendThread7, NULL, sendMessage, (void*) message7);
	pthread_create(&sendThread8, NULL, sendMessage, (void*) message8);

	//pthread_join(sendThread1, NULL);
	//pthread_join(sendThread2, NULL);
	//pthread_join(sendThread3, NULL);
	//pthread_join(sendThread4, NULL);
	//pthread_join(sendThread5, NULL);
	//pthread_join(sendThread6, NULL);
	//pthread_join(sendThread7, NULL);
	pthread_join(sendThread8, NULL);

	pthread_join(clientThread, NULL);

	return 0;

}
