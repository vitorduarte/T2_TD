/*  Transmissão de Dados
    Chat utilizando Socket TCP
    Alunos: 
            Nathalya Torres Córdova – 10/0018513 
            Pedro Matheus F. dos Santos  - 13/0060674
            Vitor Alves Duarte - 13/001846    
*/


#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <pcap.h>
//#include <libnet11/libnet.h>
#include <libnet.h>

#define IP_ADDRESS "127.0.0.1"
#define DEFAULT_PORT 9696

int PORT;
struct sockaddr_in server_addr;

/*sockaddr_in é definida em netinet/in.h
struct sockaddr_in
{
  short   sin_family; /deve ser AF_INET /
  u_short sin_port;
  struct  in_addr sin_addr; 
  char    sin_zero[8]; /Não utilizado, deve ser definida como zero/
};*/

int start_server_connection();
int start_client_connection();
int create_socket();
void client_chat(int socket_id);
void server_chat(int connection_id);
void read_socket(int socket_id, char *buffer);
void write_socket(int socket_id, char *message);
void server();
void client();
void wait_client();



int main(int argc, char **argv){

    //Verifica argumentos informados
    if (argc > 3 || argc == 1){
        printf("Erro! Numero de argumentos errado\n");
        printf("Entrada esperada: ./nhem3 -S [port]\n");
        printf("       ./gossip host [port]\n");
        printf("Se a porta não for informada a comunicação será estabelecida pela porta padrão: (9696)\n");
        return 0;
    }
    else{

        // Server
        if (!strcmp(argv[1], "-S")){    
            if(argc > 2)
                PORT = atoi(argv[2]);
            else
                PORT = DEFAULT_PORT;
            printf("\n\n--- SERVIDOR INICIALIZADO ---\n");
            printf("--- \tPORTA: %d\t  ---\n\n", PORT);
            server();


        }

        //Host
        else if (!strcmp(argv[1], "host")){   
            if(argc > 2)
                PORT = atoi(argv[2]);
            else
                PORT = DEFAULT_PORT;
            printf("\n\n--- CLIENTE CONECTADO---\n\n");
            printf("--- \tPORTA: %d  ---\n\n", PORT);
            client();
        }

        else{
            printf("Erro!\n");
            printf("Entrada esperada: ./nhem3 -S [port]\n");
            printf("\t\t  ./nhem3 host [port]\n");
        }
    }
    return 0;
}

int start_server_connection()
{
    int sockfd;
    struct sockaddr_in server_addr;

    sockfd = socket(AF_INET,SOCK_STREAM,0);

    //Se socket não for criado com sucesso informa erro
    if (sockfd < 0) 
        error("ERRO ao abrir o socket");

    bzero((char *) &server_addr, sizeof(server_addr));  //Zera o valor do buffer

    //Define configurações do servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(IP_ADDRESS);  
    server_addr.sin_port = htons(PORT);      

    bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    if(listen(sockfd, 10) == -1){
        printf("Falha na escuta do socket!\n");
        return -1;
    }

    else
    {
        printf("Conexao estabelecida!\n");
        return sockfd;
    }

}

int create_socket()
{
    int sockfd;    

    sockfd = socket(AF_INET,SOCK_STREAM,0);    

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(IP_ADDRESS);        

    return sockfd;
}

//Inicia uma conexão com um cliente
//retorna o socket criado.
int start_client_connection()
{
    int sockfd = socket(AF_INET,SOCK_STREAM,0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(IP_ADDRESS); 
    server_addr.sin_port = htons(PORT);       

    connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
  
    return sockfd;
    
}

//Inicializa um novo servidor
void server()
{    
    int connection_id = 0;
    int socket_id = start_server_connection();

    connection_id = accept(socket_id, (struct sockaddr*)NULL ,NULL);
    wait_client(connection_id);
    server_chat(connection_id);
    close(connection_id);

    close(socket_id);
}

//Inicializa um novo cliente
void client()
{
    int socket_id = 0;

    socket_id = start_client_connection();
    client_chat(socket_id);
    close(socket_id);    
}

/*  --- Chat do cliente ---

@socket_id identificador de socket para comunicação
o envio e recebimento de mensagens só é estabelecido após o usuário digitar "OI SRV"
o envio e recebimento de mensagens é interrompido ao receber "XAU CLT" do servidor  
*/
void client_chat(int socket_id)
{
    char buffer[1025];
    char str[30];
    buffer[0] = '\0';
    str[0] = '\0';


    while(strcmp(str, "OI SRV\n"))
    {
        fgets( str , 29, stdin);
    }
    write_socket(socket_id, str);
    read_socket(socket_id, buffer);  
    while(strcmp(buffer, "XAU CLT\n"))
    {
        fgets( str , 29, stdin);
        write_socket(socket_id, str);
        read_socket(socket_id, buffer);        
    }

    write_socket(socket_id, "XAU SRV\n");

}

//Espera chamado "OI SRV" para responde-lo confirmando a comunicação
void wait_client(int connection_id)
{
    char buffer[1025];
    buffer[0] = '\0';
    while(strcmp(buffer, "OI SRV\n"))
    {
        read_socket(connection_id,buffer);        
    }
    write_socket(connection_id, "OI CLT\n");

}

/*  --- Chat do servidor ---
    @conn_id identificador do socket para a comunicação
    se for digitado no terminal do servidor "XAU CLT" o programa é interrrompido  
*/
void server_chat(int conn_id)
{
    char buffer[1025];
    char str[30];
    buffer[0] = '\0';

    while(strcmp(buffer, "XAU SRV\n"))
    {
        read_socket(conn_id, &buffer[0]); 
        if(!strcmp(buffer, "XAU SRV\n")){       
            break;   
        }
        fgets( str , 29, stdin);
        write_socket(conn_id, str);
    }
}

//Realiza a leitura do socket
void read_socket(int socket_id, char *buffer)
{
    char b[1025];
    int read_size = read(socket_id, b, sizeof(b));
    b[read_size] = '\0';
    strcpy(buffer, b);
    fprintf(stderr, "\t\t %s\n", b);    
}

//Realiza a escrita no socket
void write_socket(int socket_id, char *message)
{
    write(socket_id, message, strlen(message));
}