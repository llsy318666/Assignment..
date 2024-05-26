#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define BUFFER_SIZE 100
#define NP_SEND "./server_to_client"
#define NP_RECEIVE "./client_to_server"

int main(void) {
	char receive_msg[BUFFER_SIZE], send_msg[BUFFER_SIZE];
	int receive_fd, send_fd;
	int value;

	
	if (access(NP_SEND, F_OK) == 0) {
		unlink(NP_SEND);
	}
	if (access(NP_RECEIVE, F_OK) == 0) {
		unlink(NP_RECEIVE);
	}
	mkfifo(NP_RECEIVE, 0666);
	mkfifo(NP_SEND, 0666);
	receive_fd = open(NP_RECEIVE, O_RDONLY);
	send_fd = open(NP_SEND, O_WRONLY);

	while (1) {

		ssize_t bytes_read = read(receive_fd, receive_msg, BUFFER_SIZE);
		if (bytes_read == 0) { break; }
		printf("server : receive %s\n", receive_msg);

		value = atoi(receive_msg);

		sprintf(send_msg, "%d", value*value);
		printf("server : send %s\n", send_msg);

		write(send_fd, send_msg, strlen(send_msg) + 1);

	}
	return 0;
}