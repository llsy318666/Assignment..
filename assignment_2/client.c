#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define BUFFER_SIZE 100
#define NP_RECEIVE "./server_to_client"
#define NP_SEND "./client_to_server"

int main(void) {
	char receive_msg[BUFFER_SIZE], send_msg[BUFFER_SIZE];
	int receive_fd, send_fd;

	
	send_fd = open(NP_SEND, O_WRONLY);
	receive_fd = open(NP_RECEIVE, O_RDONLY);


	for (int i=1; i<10; i++) {
		printf("client : send %d\n", i);
		sprintf(send_msg, "%d", i);

		write(send_fd, send_msg, strlen(send_msg) + 1);
		read(receive_fd, receive_msg, BUFFER_SIZE);


		printf("client : receive %s\n\n", receive_msg);

        	usleep(500*1000);
	}
	return 0;
}