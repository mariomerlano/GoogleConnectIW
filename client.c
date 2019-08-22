#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>


//char* textoImprimir char*buffer
#define ConsoleGetString(textoImprimir,buffer) printf("%s: \n",textoImprimir);\
						 scanf("%s", buffer,sizeof(buffer));

#define s(string) printf("%s \n",string);
#define BUFFLEN sizeof(struct miProtocolo)
// //Escucha para recibir strings de caracteres variados
// void MandarVarString(int fileDescriptor, char* string)
// {
// 	int longitudString = strlen(string);
// 	send(fileDescriptor, &longitudString, sizeof(longitudString), 0);
// 	send(fileDescriptor,(void*)string,strlen(string), 0);
// }



// //Escucha para recibir strings de caracteres variados
// char *RecibirVarString(int fileDescriptor)
// {
// 	int longitudString = 0;
// 	recv(fileDescriptor, &longitudString, sizeof(longitudString), 0);
// 	char *buffer = malloc(longitudString);
// 	recv(fileDescriptor, buffer, longitudString, 0);
// 	return buffer;
// }

struct miProtocolo
{
	int numero1;
	int numero2;
	int resultado;
};

//Extension del send
void SendAll(int sd, int* buffer, int totalLength)
{
	int bytes=0;
	while (totalLength>0)
	{
		bytes=send(sd,buffer,totalLength,0);
		//printf("Mande %d bytes \n",bytes);
		if ((bytes<=0)){break;}
		totalLength-=bytes;
	}

	(totalLength>0)?perror("Error en send all"):((void*)0);
		
}

//Manda un string de cualquier longitud
void SendString(int sd,char* string)
{
	int longitudString = strlen(string);
	send(sd, &longitudString, sizeof(longitudString), 0);
	SendAll(sd,string,longitudString);
}

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
	int sd;
	char teclado[512];
	char buffer[10];
	struct sockaddr_in servidor;
	struct sockaddr_in cliente;
	struct hostent *h;
	struct miProtocolo miProto;

	if (argc < 2) {
		printf("Debe ejecutar %s (nombre de host)\n",argv[0]); 
		exit (-1);
	}

	sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	servidor.sin_family = AF_INET;
	servidor.sin_port = htons(4440);
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
		// printf("Ingrese ALGO: ");
		// fgets(teclado, sizeof(teclado), stdin);
		// teclado[strlen(teclado) - 1] = '\0';

		// miProto->miString = htonl(atoi(teclado));

		// send(sd, buffer, BUFFLEN, 0);
		// recv(sd , buffer, BUFFLEN, 0);
		// //n = leer_mensaje (sd, buffer, BUFFLEN );
		// printf("el sv me tiro %d\n", ntohl(miProto->miString));

	char* mensaje=RecevString(sd);
	s(mensaje);
	free(mensaje);
	close(sd);

}