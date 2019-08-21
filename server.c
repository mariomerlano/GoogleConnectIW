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
	int numero1;
	int numero2;
	int resultado;
};

//Extension del recv
void RecvAll(int sd,void* buffer, int totalLength)
{
	int bytes=0;
	int recibido=0;
	while (recibido!=totalLength)
	{
		bytes=recv(sd,buffer+recibido,totalLength-recibido,0);
		//printf("Recibi %d bytes \n",bytes);
		  if (bytes < 0)
            perror("Error leyendo respuesta");
        if (bytes == 0)
            break;
		recibido+=bytes;
	}
}

//Recive un string de cualquier longitud
char* RecevString(int sd)
{
	int longitudString = 0;
	recv(sd, &longitudString, sizeof(longitudString), 0);
	char *buffer = malloc(longitudString);
	RecvAll(sd, buffer, longitudString);
	return buffer;
}

int main(int argc, char *argv[]) {
	int n;
	int sd; // socket descriptor server
	int sdc; // socket descriptor client
	int lon;
	char buffer[BUFFLEN];
	struct sockaddr_in servidor;
	struct sockaddr_in cliente;
	struct miProtocolo miProto;
	
	servidor.sin_family = AF_INET; //seteamos IPv4
	servidor.sin_port = htons(4445);
	servidor.sin_addr.s_addr = INADDR_ANY;
	sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (bind(sd, (struct sockaddr *) &servidor, sizeof(servidor)) < 0) {
		perror("Error en bind");
		exit(-1);
	}
	
	listen(sd, 5);
	
	//for(;;) {
		lon = sizeof(cliente);
		sdc = accept(sd, (struct sockaddr *)&cliente, &lon);
			// recv(sdc, buffer, BUFFLEN, 0);
			// printf("Recibí desde: %s puerto: %d \n", inet_ntoa(cliente.sin_addr), ntohs(cliente.sin_port));
			// printf("Me llego desde el cliente %d ", ntohl(miProto->miString));
			// mfiProto->miString = htonl(122);
		
		char* string=RecevString(sdc);
		printf("Mensaje: %s \n",string);
		//free(buff);
		close(sdc);
	//}
	close(sd);
	return 1;
}