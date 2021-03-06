#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "port.h"

int
main(int argc, char **argv)
{
	struct sockaddr_in myaddr;	/* our address */
	struct sockaddr_in remaddr;	/* remote address */
	socklen_t addrlen = sizeof(remaddr);		/* length of addresses */
	int recvlen;			/* # bytes received */

	/* create a UDP socket */
	int fd;				/* our socket */
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("cannot create socket\n");
		return 0;
	}

	/* bind the socket to any valid IP address and a specific port */

	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(SERVICE_PORT);

	if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
		perror("bind failed");
		return 0;
	}

	/* now loop, receiving data and printing what we received */
	char* buf_recv=(char*)malloc(1034);
	for (;;) {
		printf("waiting on port %d\n", SERVICE_PORT);
		recvlen = recvfrom(fd, buf_recv, 1034, 0, (struct sockaddr *)&remaddr, &addrlen);
		if (recvlen > 0) {
			// CEK IS MESSAGE ERROR OR NOT
			// if(!iserror)
			int data_length = (buf_recv[5] << 24 ) | (buf_recv[6] << 16) | (buf_recv[7] << 8) | buf_recv[8];
			char checksum = 0;
			for(int i = 0; i < data_length+10; i++){
				checksum +=  buf_recv[i];
			}
			printf("%x\n", checksum);
			if(checksum == 0xffffffff){
				printf("no error\n");
			}else{
				printf("error while transmit\n");
			}

			// PRINT THE DATA RECEIVED
			
			printf("received message (%d byte data): ", data_length);
			for(int i = 0; i < data_length; i++){
				printf("%c", buf_recv[9+i]);
			}
			printf("\n");
			// printf("received message: \"%s\" (%d bytes)\n", buf_recv, recvlen);
		}
		else
			printf("uh oh - something went wrong!\n");
		char* buf_send=(char*)malloc(6);
		sprintf(buf_send, "ACK");
		printf("sending response \"%s\"\n", buf_send);
		if (sendto(fd, buf_send, 6, 0, (struct sockaddr *)&remaddr, addrlen) < 0)
			perror("sendto");
	}
	/* never exits */
}
