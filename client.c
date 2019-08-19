#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#define BUFLEN sizeof(struct miProtocolo)

struct miProtocolo
{
	uint32_t miNumero;
};

int main(int argc, char *argv[]) {
	int n;
	int sd;
	char teclado[512];
	char buffer[BUFLEN];
	struct sockaddr_in servidor;
	struct sockaddr_in cliente;
	struct hostent *h;
	struct miProtocolo* miProto;
	

	if (argc < 2) {
		printf("Debe ejecutar %s (nombre de host)\n",argv[0]); 
		exit (-1);
	}

	sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	servidor.sin_family = AF_INET;
	servidor.sin_port = htons(4444);
	//servidor.sin_addr.s_addr = inet_addr("x.x.x.x");

	if ( h = gethostbyname(argv [1])) {
		memcpy ( &servidor.sin_addr , h->h_addr , h->h_length );
        } else {
                herror("DNS error");
                exit (-1);
	}

	if (connect(sd, (struct sockaddr*)&servidor, sizeof(servidor)) < 0) {
		perror ("Error en connect");
		exit(-1);
	}

	miProto = (struct miProtocolo*)buffer;

		printf("Ingrese algun numero: ");
		fgets(teclado, sizeof(teclado), stdin);
		teclado[strlen(teclado) - 1] = '\0';

		miProto->miNumero = htonl(atoi(teclado));

		send(sd, buffer, BUFLEN, 0);
		recv(sd , buffer, BUFLEN, 0);
		//n = leer_mensaje (sd, buffer, BUFLEN );
		printf("el sv me tiro %d\n", ntohl(miProto->miNumero));

	close(sd);

}