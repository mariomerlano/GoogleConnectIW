#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define BUFFLEN sizeof(struct miProtocolo)

struct miProtocolo
{
	int miString;
};

int main() {

	1!=1 ? printf("%d", 33) : printf("%d", 22);
	int n;
	int sd; // socket descriptor server
	int sdc; // socket descriptor client
	int lon;
	char buffer[BUFFLEN];
	struct sockaddr_in servidor;
	struct sockaddr_in cliente;
	struct miProtocolo* miProto;
	
	servidor.sin_family = AF_INET; //seteamos IPv4
	servidor.sin_port = htons(4444);
	servidor.sin_addr.s_addr = INADDR_ANY;
	sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (bind(sd, (struct sockaddr *) &servidor, sizeof(servidor)) < 0) {
		perror("Error en bind");
		exit(-1);
	}
	
	listen(sd, 5);
	miProto = (struct miProtocolo*)buffer;
	for(;;) {
		lon = sizeof(cliente);
		sdc = accept(sd, (struct sockaddr *)&cliente, &lon);
			recv(sdc, buffer, BUFFLEN, 0);
			printf("Recibí desde: %s puerto: %d \n", inet_ntoa(cliente.sin_addr), ntohs(cliente.sin_port));
			printf("Me llego desde el cliente %d ", ntohl(miProto->miString));
			miProto->miString = htonl(122);

			send(sdc, buffer, BUFFLEN, 0);
		
		close(sdc);
	}
	close(sd);
}