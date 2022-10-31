#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080

int main(int argc, char const* argv[]) {

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

	return 0;

}
