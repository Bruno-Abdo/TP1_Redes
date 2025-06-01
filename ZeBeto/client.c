#include "common.h"

GameMessage msg;

// Escolha de jogada do cliente
int jogada;

// Variável para armazenar o socket
int socket_cliente;

// Struct do tipo "sockaddr_storage" para suportar IPv4 ou IPv6 quando chamada
struct sockaddr_storage endereco_cliente;

// Variável para armazenar o tamanho do endereço (IPv4 ou IPv6)
socklen_t tamanho_endereco;

// Variável para determinar o IP
int dominio;

int main(int argc, char *argv[]){

    // Usage 
    if (argc != 3) {
        printf("Usage: %s <server IP> <server port> \n", argv[0]);
        return 1;
    }

    // Determina o IP usado pelo cliente
    determina_ip(argv[1], &dominio);

    // Inicializa o socket 
    inicializa_socket_c(dominio, &socket_cliente);

    // Configura as estruturas de endereco
    indicador_porta_c(dominio, atoi(argv[2]), &endereco_cliente, &tamanho_endereco);

    // Funcao connect e seu tratamento de erro
    connect(socket_cliente, (struct sockaddr *)&endereco_cliente, tamanho_endereco);
    printf("Conectado ao servidor\n");

    do {
        recebeMensagem(socket_cliente, &msg);
    
    } while (msg.type != MSG_END);

    close(socket_cliente);
    return 0;

}