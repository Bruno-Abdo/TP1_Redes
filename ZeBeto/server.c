#include "common.h"

GameMessage msg;

char *jogadaCliente;
char *jogadaservidor;


// Variáveis para armazenar os sockets
int socket_servidor;
int socket_cliente;

// Structs do tipo "sockaddr_storage" para suportarem IPv4 ou IPv6 quando chamadas 
struct sockaddr_storage endereco_servidor;
struct sockaddr_storage endereco_cliente;

// Variável para armazenar o tamanho do endereço (IPv4 ou IPv6)
socklen_t tamanho_endereco;

int main(int argc, char *argv[]){

    // Usage 
    if (argc != 3) {
        printf("Usage: %s <v4|v6> <server port> \n", argv[0]);
        return 1;
    }

    // Inicializa o socket
    inicializa_socket_s(argv[1], &socket_servidor);

    // Configura as estruturas de endereço
    indicador_porta_s(argv[1], atoi(argv[2]), &endereco_servidor, &tamanho_endereco);

    // Confirma em qual endereço e porta o servidor foi iniciado
    mensagem_confirmacao(argv[1], argv[2]);

    // Função bind e seu tratamento de erro
    if(bind(socket_servidor, (struct sockaddr *)&endereco_servidor, tamanho_endereco) < 0){
        printf("Erro bind\n");
    }

    // Função listen e seu tratamento de erro
    if(listen(socket_servidor, 1) < 0){
        printf("Erro listen\n");
    }

    // Função accept e seu tratamento de erro
    if((socket_cliente = accept(socket_servidor, (struct sockaddr *)&endereco_cliente, &tamanho_endereco)) < 0){
        printf("Erro no accept\n");
    }

    // Confirmação de conexão do cliente
    printf("Cliente conectado.\n");

    exibe_jogadas(&msg, &socket_cliente);

    do {
        recebeMensagem(socket_cliente, &msg);
    
    } while (msg.type != MSG_END);



    // Encerra as conexões
    close(socket_servidor);
    close(socket_cliente);

    return 0;
}