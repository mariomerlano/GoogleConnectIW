#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>   
#include <stdlib.h> 
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "fetch.h"  

#define TRUE 1
#define FALSE 0
#define s(string) printf("%s \n",string);
#define d(numero) printf("%d \n",numero);
#define RESPONSEMAX 40960
//Abreviacion para el connect
#define SHORTCONNECT(sd,servidor)\  
    if (connect(sd, (struct sockaddr*)&servidor, sizeof(servidor)) < 0)\ 
    {\
	perror ("Error en connect");\
	exit(-1);} else{s("Conectado!")\
    }

#define ERRORHTTPS(err) if (err != 1) \
    {\
    unsigned long a=SSL_get_error(conn,err);\
    char buffer[3000];\
    ERR_error_string(a,&buffer);\
    s(buffer);\
    abort(); \
    }
//CODIGO UTIL
//Saca la ip que tiene guardada el sockaddr_in
// char str[INET_ADDRSTRLEN];
// inet_ntop(AF_INET, &(servidor.sin_addr), str, INET_ADDRSTRLEN);
//Extension del send

struct response 
{
    int status;
    char* header;
    char* body;
};

//No olvidarse de hacer free de las respuestas
char* GetHostPathNames(char* link,char** hostName,char** path){
    int longitud=0;
    int longInicial=(strstr(link, "https"))?8:7;
    int lengthStr=strlen(link);
    //Sacamos la longitud del string del host
    for (size_t i = longInicial; i < lengthStr; i++)
    {
        char actual=link[i];
        if (actual=='/'){ break;}

        longitud++;
    }

    *hostName=malloc(longitud+1);
    memcpy(*hostName,link+longInicial,longitud);
    *path=malloc(lengthStr-longInicial+longitud+1);
    memcpy(*path,link+longInicial+longitud,lengthStr-longInicial+longitud);
    
}

//Extension del send
void SendAll(int sd, int* buffer, int totalLength)
{
	int bytes=0;
	while (totalLength>0)
	{
		bytes=write(sd,buffer,totalLength);
		//printf("Mande %d bytes \n",bytes);
		if ((bytes<=0)){break;}
		totalLength-=bytes;
	}

	(totalLength>0)?perror("Error en send all"):((void*)0);
		
}

//Extension del recv
void RecvAll(int sd,void* buffer, int totalLength)
{
	int bytes=0;
	int recibido=0;
	while (recibido!=totalLength)
	{
		bytes=read(sd,buffer+recibido,totalLength-recibido);
		//printf("Recibi %d bytes \n",bytes);
		  if (bytes < 0)
            perror("Error leyendo respuesta");
          if (bytes == 0)
            break;
		recibido=recibido+bytes;
	}
}

//Devuelve la ip a partir del nombre de dominio
void GetIP(char* hostName,struct in_addr* s_addr)
{
    struct hostent* h;
    if (h = gethostbyname(hostName))
    {
        memcpy(s_addr, h->h_addr, h->h_length);
    }
    else
    {
        herror("DNS error");
        exit(-1);
    }
}

//Inicializa sockaddr_in para IPV4
struct sockaddr_in sockInit(int puerto){
    struct sockaddr_in servidor;
    servidor.sin_family=AF_INET;
    servidor.sin_port=htons(puerto);
    servidor.sin_addr.s_addr="";
    return servidor;
}

//Un clasico fetch de js [Solo hace GET por ahora]
struct response fetchGet(char* request)
{
    char *hostName;
    char *path;
    int https=strstr(request, "https");
    //Creo el socket
    int sd = socket(PF_INET, SOCK_STREAM, 0);
    //Inicializo el sockaddr y cargo la ip del dominio. El puerto depende si es HTTPS o HTTP
    struct sockaddr_in servidor=sockInit(https?443:80);
    //Sacamos el nombre del host
    GetHostPathNames(request,&hostName,&path);
    s(hostName);
    s(path);
    //Sacamos la ip del host
    GetIP(hostName,&servidor.sin_addr);
    //Hago el connect
    SHORTCONNECT(sd,servidor);
    char* message="GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n";
    request=malloc(strlen(message)+strlen(hostName)+strlen(path));
    sprintf(request,message,path,hostName);
    char* response=malloc(RESPONSEMAX);
    
    if (https)
    {
    // Inicializo el sockSSL
    SSL_load_error_strings();
    SSL_library_init();
    SSL_CTX *ssl_ctx = SSL_CTX_new(SSLv23_client_method());
    // Creo la conexion SSL y la adjunto al socket
    SSL *conn= SSL_new(ssl_ctx);
    SSL_set_fd(conn, sd);
    int err = SSL_connect(conn);
    //Check para saber si hubo error en la conexion
    ERRORHTTPS(err);

    //Cantidad de bytes enviados/recibidos
    int cantidad;
    if(!SSL_write_ex(conn,request,strlen(request),&cantidad)){s("Hubo un error en send HTTPS")};
    if(!SSL_read_ex(conn,response,RESPONSEMAX,&cantidad)){s("Hubo un error en recv HTTPS")};
    //Liberamos el bindeo del socket y la conexion ssl
    SSL_free(conn);
    }
    else
    {
         SendAll(sd,request,strlen(request));
         RecvAll(sd,response,RESPONSEMAX);
    }
    free(hostName);
    free(path);
    //Cerramos conexion
    close(sd);
    //printf(response);
    int empiezaBody=strlen(response);
    for (size_t i = 0; i < strlen(response); i++)
    {
        //168626701 en entero es \r\n\r\n
        //uso entero asi el for va fijandose de a 4 bytes
        if (*(int*)(response+(int)i)==168626701)
        {
            empiezaBody=i+4;
        }
        
    }
    struct response responseAux;
    //Cargamos la estructura
    char* statusStr=malloc(4);
    char* header=malloc(empiezaBody+1);
    char* body=malloc(strlen(response)+1-empiezaBody);
    memcpy(statusStr,response+9,4);
    memcpy(header,response,empiezaBody);
    memcpy(body,response+empiezaBody,strlen(response)-empiezaBody);
    responseAux.status=atoi(statusStr);
    responseAux.header=header;
    responseAux.body=body;
    free(statusStr);
    free(response);
    return responseAux;
}

int main()
{
    struct response response=fetchGet("https://api.mercadolibre.com/items/MLA808682506");
    s(response.body);
    //s(response.header);
    free(response.header);
    free(response.body);
    //s(response);
    
}


