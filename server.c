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
#include <semaphore.h> 
#include "./fetch.h" 
/*
Comando para compilar:
sudo gcc server.c fetch.c -o foo -lssl -lcrypto -pthread
*/
#define BUFFLEN sizeof(struct miProtocolo)
// 1 si queremos debugear
// 0 si no queremos debugear
#define DEBUG 1
#if DEBUG
	#define PS(x) printf("%s \n", x); // imprimir string
	#define PI(x) printf("%d \n", x); // imprimir int
#else
	#define PS(x)
	#define PI(x)
#endif // DEBUG

struct miProtocolo
{
	int numero1;
	int numero2;
	int resultado;
};

//Recibe un string de cualquier longitud
char* RecevString(int sd);

//Manda un string de cualquier longitud
void SendString(int sd,char* string);

void *AtenderCliente(void *sdcarg);
void *AdministradorClientes(void *sdarg);

int SetupServer();

sem_t semaforo; 
int main(int argc, char *argv[]) {
	pthread_t tid_admin;
	int sd = SetupServer();
	pthread_create(&tid_admin, NULL, AdministradorClientes, (void *)&sd); //admin de clientes
	pthread_join(tid_admin, NULL);
	close(sd);
	return 1;
}

char* RecevString(int sd)
{
	int longitudString = 0;
	recv(sd, &longitudString, sizeof(longitudString), 0);
	char *buffer = malloc(longitudString);
	RecvAll(sd, buffer, longitudString);
	return buffer;
}

//Manda un string de cualquier longitud
void SendString(int sd,char* string)
{
	int longitudString = strlen(string);
	send(sd, &longitudString, sizeof(longitudString), 0);
	SendAll(sd,string,longitudString);
}

void *AdministradorClientes(void *sdarg){
	sem_init(&semaforo, 0, 1); 
    pthread_t tid_client;
	struct sockaddr_in cliente;
	int sdc; // socket descriptor client
	int len;
	int sd = *(int *)sdarg;
    PS("Soy el admin con sd") PI(sd)
	listen(sd, 5);
	for(;;) {
		len = sizeof(cliente);
		sdc = accept(sd, (struct sockaddr *)&cliente, &len);
		pthread_create(&tid_client, NULL, AtenderCliente, (void *)&sdc); 
	} 
    pthread_join(tid_client, NULL); //es como el wait, bloquea el flujo del llamante. Usarlo para q no queden clientes colgados
	sem_destroy(&semaforo); 
}

void *AtenderCliente(void *sdcarg) 
{ 
	sem_wait(&semaforo); 

	//seccion critica
    int sdc = *(int *)sdcarg; 
	// recv(sdc, buffer, BUFFLEN, 0);
	// printf("Recibí desde: %s puerto: %d \n", inet_ntoa(cliente.sin_addr), ntohs(cliente.sin_port));
	// printf("Me llego desde el cliente %d ", ntohl(miProto->miString));
	// mfiProto->miString = htonl(122);
	struct response response=fetchGet("https://blockchain.info/q/24hrprice");
    int valorBTC=atoi((char*)(response.body+3));
	char* template="El valor actual del btc es: $%d usd\n";
	char* mensaje=malloc(strlen(template)+12);
	sprintf(mensaje,template,valorBTC);
	SendString(sdc,mensaje);
	//printf("Mensaje: %s \n",string);
	//free(buff); 
	PS("Sd cliente nro: ") PI(sdc)
	free(response.header);
    free(response.body);
	close(sdc);
    sleep(5); // simulamos q tarda
	//fin seccion critica

	sem_post(&semaforo); 
} 

int SetupServer(){
	int sd; // socket descriptor server
	char buffer[BUFFLEN];
	struct sockaddr_in servidor;
	struct miProtocolo miProto;
	
	servidor.sin_family = AF_INET; //seteamos IPv4
	servidor.sin_port = htons(4440);
	servidor.sin_addr.s_addr = INADDR_ANY;
	sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (bind(sd, (struct sockaddr *) &servidor, sizeof(servidor)) < 0) {
		perror("Error en bind");
		exit(-1);
	}
	PS("Setup hechos!");
	PI(sd);
	return sd;
}