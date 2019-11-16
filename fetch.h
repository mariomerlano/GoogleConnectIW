#ifndef Fetch   
#define Fetch

struct response 
{
    int status;
    char* header;
    char* body;
};

//Extension del recv
void RecvAll(int sd,void* buffer, int totalLength);
//Extension del Send
void SendAll(int sd, int* buffer, int totalLength);
struct response fetchGet(char* dominio);  

#endif 