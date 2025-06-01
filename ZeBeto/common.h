// Arquivo contendo algumas funções e bibliotecas utilizadas em ambos os códigos (servidor e cliente) 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>


#define MSG_SIZE 256

// Enum para o tipo de mensagem
typedef enum {
    MSG_REQUEST,
    MSG_RESPONSE,
    MSG_RESULT,
    MSG_PLAY_AGAIN_REQUEST,
    MSG_PLAY_AGAIN_RESPONSE,
    MSG_ERROR,
    MSG_END
} MessageType;

// Struct que armazena as informações da mensagem 
typedef struct {
    int type;
    int client_action;
    int server_action;
    int result;
    int client_wins;
    int server_wins;
    char message[MSG_SIZE];
} GameMessage;

int escolha_aleatoria(){

        static int inicializado = 0;
        if (!inicializado) {
            srand(time(NULL));
            inicializado = 1;
        }
        return rand() % 5; // 0-4
}

int inicializa_socket_s(char *dominio_terminal, int *meu_socket) {  
    // Função que inicializa o socket com base no tipo de IP (IPv4 ou IPv6)
    int dominio_funcao;  // Variável para armazenar o domínio (AF_INET ou AF_INET6)

    if (strcmp(dominio_terminal, "v4") == 0) {  // Compara a string passada com "v4"
        dominio_funcao = AF_INET;                // Domínio para IPv4
    }
    else if (strcmp(dominio_terminal, "v6") == 0) {  // Compara a string passada com "v6"
        dominio_funcao = AF_INET6;                  // Domínio para IPv6
    } 

    *meu_socket = socket(dominio_funcao, SOCK_STREAM, 0);  // Cria o socket TCP com o domínio selecionado

    if(*meu_socket < 0){
        printf("Erro socket.\n");
    }

    return *meu_socket;  // Retorna o valor do socket criado
}

int inicializa_socket_c(int dominio_terminal, int *meu_socket) {  
    // Função que inicializa o socket com base no tipo de IP (IPv4 ou IPv6)

    *meu_socket = socket(dominio_terminal, SOCK_STREAM, 0);  // Cria o socket TCP com o domínio selecionado

    if(*meu_socket < 0){
        printf("Erro socket.\n");
    }

    return *meu_socket;  // Retorna o valor do socket criado
}

void indicador_porta_s(char *dominio_socket, int porta, struct sockaddr_storage *endereco, socklen_t *tamanho_endereco) {
    // Função que configura a estrutura de endereço para IPv4 ou IPv6

    int dominio_funcao;  // Variável para armazenar o domínio (AF_INET ou AF_INET6)

    if (strcmp(dominio_socket, "v4") == 0) {  
        dominio_funcao = AF_INET;  // Domínio para IPv4
    }
    else if (strcmp(dominio_socket, "v6") == 0) {
        dominio_funcao = AF_INET6;  // Domínio para IPv6
    } 

    if (dominio_funcao == AF_INET) {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)endereco;  // Cast para sockaddr_in
        addr4->sin_family = AF_INET;
        addr4->sin_port = htons(porta);  // Converte a porta para formato de rede
        addr4->sin_addr.s_addr = INADDR_ANY;  // Aceita conexões de qualquer IP
        *tamanho_endereco = sizeof(struct sockaddr_in);  // Define o tamanho correto
    } else if (dominio_funcao == AF_INET6) {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)endereco;  // Cast para sockaddr_in6
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = htons(porta);  // Converte a porta para formato de rede
        addr6->sin6_addr = in6addr_any;  // Aceita conexões de qualquer IP (IPv6)
        *tamanho_endereco = sizeof(struct sockaddr_in6);  // Define o tamanho correto
    }
}

void indicador_porta_c(int dominio_socket, int porta, struct sockaddr_storage *endereco, socklen_t *tamanho_endereco) {
    // Função que configura a estrutura de endereço para IPv4 ou IPv6

    if (dominio_socket == AF_INET) {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)endereco;  // Cast para sockaddr_in
        addr4->sin_family = AF_INET;
        addr4->sin_port = htons(porta);  // Converte a porta para formato de rede
        addr4->sin_addr.s_addr = INADDR_ANY;  // Aceita conexões de qualquer IP
        *tamanho_endereco = sizeof(struct sockaddr_in);  // Define o tamanho correto
    } else if (dominio_socket == AF_INET6) {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)endereco;  // Cast para sockaddr_in6
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = htons(porta);  // Converte a porta para formato de rede
        addr6->sin6_addr = in6addr_any;  // Aceita conexões de qualquer IP (IPv6)
        *tamanho_endereco = sizeof(struct sockaddr_in6);  // Define o tamanho correto
    }
}

void mensagem_confirmacao(char *dominio_terminal, char *porta) {  
    // Função que exibe a mensagem de confirmação de início do servidor

    if (strcmp(dominio_terminal, "v4") == 0) {  
        printf("Servidor iniciado em modo IPv4 na porta %s. Aguardando conexão...\n", porta);
    }
    else if (strcmp(dominio_terminal, "v6") == 0) {
        printf("Servidor iniciado em modo IPv6 na porta %s. Aguardando conexão...\n", porta);
    } 
}

void determina_ip(char *ip, int *dominio) {   
    // Determina o tipo de IP com base no argumento dado no terminal

    if (strchr(ip, ':') != NULL) {
        *dominio = AF_INET6;  // IPv6
    } else {
        *dominio = AF_INET;   // IPv4
    }
}

void enviaJogarNovamente(int socket, GameMessage *msg) {
    int escolha;

    scanf("%d", &escolha);
    printf("\n");

    // Preenche a estrutura da mensagem
    msg->type = MSG_PLAY_AGAIN_RESPONSE;     
    msg->result = escolha; 

    // Envia a mensagem
    if (send(socket, msg, sizeof(GameMessage), 0) < 0) {
        printf("Erro ao enviar mensagem\n");
    }

}

void encerrando_conexao(GameMessage *msg, int meu_socket) {  
    msg->type = MSG_END;

    snprintf(msg->message, MSG_SIZE,
        "Fim de jogo!\n"
        "Placar final: Você %d x %d Servidor\n"
        "Obrigado por jogar!\n",
        msg->client_wins,
        msg->server_wins
    );

    if (send(meu_socket, msg, sizeof(GameMessage), 0) < 0) {
        printf("Erro no send.\n");
    }

}

void determina_resultado(GameMessage *msg) {  
    // Função que determina o resultado do jogo
    // 0 = Empate, 1 = Vitoria, -1 = Derrota

    if (msg->client_action == msg->server_action) {
        msg->result = 0;  // Empate
        return;
    }

    switch (msg->client_action) {
        case 0:  // Nuclear Attack
            if (msg->server_action == 2 || msg->server_action == 3) {
                msg->result = 1;   // Cliente vence
            } else {
                msg->result = -1;  // Servidor vence
            }
            break;

        case 1:  // Intercept Attack
            if (msg->server_action == 0 || msg->server_action == 4) {
                msg->result = 1;
            } else {
                msg->result = -1;
            }
            break;

        case 2:  // Cyber Attack
            if (msg->server_action == 1 || msg->server_action == 3) {
                msg->result = 1;
            } else {
                msg->result = -1;
            }
            break;

        case 3:  // Drone Strike
            if (msg->server_action == 1 || msg->server_action == 4) {
                msg->result = 1;
            } else {
                msg->result = -1;
            }
            break;

        case 4:  // Bio Attack
            if (msg->server_action == 0 || msg->server_action == 2) {
                msg->result = 1;
            } else {
                msg->result = -1;
            }
            break;

    }
}

void calculoResultado(GameMessage *msg, char *resultado){
    if (msg->result == 1) {
        strcpy (resultado, "Vitoria!");
    }
    else if (msg->result == 0) {
        strcpy (resultado,"Empate!");
    }
    else if (msg->result == -1) {
        strcpy (resultado, "Derrota!");
    }
}

void exibe_jogadas(GameMessage *msg, int *meu_socket) {
    // Define o tipo da mensagem
    msg->type = MSG_REQUEST;

    // Preenche o campo message com o menu de jogadas
    snprintf(msg->message, MSG_SIZE,
        "Escolha sua jogada:\n\n"
        "0 - Nuclear Attack\n"
        "1 - Intercept Attack\n"
        "2 - Cyber Attack\n"
        "3 - Drone Strike\n"
        "4 - Bio Attack\n"
    );

    if (send(*meu_socket, msg, sizeof(GameMessage), 0) < 0) {
        printf("Erro no send.\n");
    }
    printf("Apresentando as opções para o cliente.\n");

}

void jogarNovamente(int *socket, GameMessage *msg) {
    msg->type = MSG_PLAY_AGAIN_REQUEST;

    snprintf(msg->message, MSG_SIZE,
        "Deseja jogar novamente?\n"
        "1 - Sim\n"
        "0 - Não\n"
    );

    if (send(*socket, msg, sizeof(GameMessage), 0) < 0) {
        printf("Erro no send.\n");
    }

    printf("Perguntando se o cliente deseja jogar novamente.\n");

}

void calculaHistorico(GameMessage *msg) {
    switch(msg->result){
        case -1:
            msg->server_wins += 1;
            printf("Placar atualizado: Cliente %d x %d Servidor\n", msg->client_wins, msg->server_wins);
            break;

        case 1:
            msg->client_wins += 1;
            printf("Placar atualizado: Cliente %d x %d Servidor\n", msg->client_wins, msg->server_wins);
            break;

        case 0:
            printf("Jogo empatado.\n");
            printf("Solicitando ao cliente mais uma escolha.\n");
            break;

    }
}

void enviaEscolha(int socket, GameMessage *msg) {
    int escolha;

    scanf("%d", &escolha);
    printf("\n");

    // Preenche a estrutura da mensagem
    msg->type = MSG_RESPONSE;     
    msg->client_action = escolha; 

    // Envia a mensagem
    if (send(socket, msg, sizeof(GameMessage), 0) < 0) {
        printf("Erro ao enviar mensagem\n");
    }
}

void invalidaJogarNovamente(GameMessage *msg, int socket_cliente){
    msg->type = MSG_ERROR; 
    snprintf(msg->message, MSG_SIZE, "Por favor, digite 1 para jogar novamente ou 0 para encerrar.\n");

    if (send(socket_cliente, msg, sizeof(GameMessage), 0) < 0) {
        printf("Erro ao enviar mensagem de escolha inválida\n");
    }

    enviaJogarNovamente(socket_cliente, msg);

}

void jogadaInvalida(GameMessage *msg, int *socket_cliente) {

    msg->type = MSG_ERROR; 
    snprintf(msg->message, MSG_SIZE, "Por favor, selecione um valor de 0 a 4.\n");

    if (send(*socket_cliente, msg, sizeof(GameMessage), 0) < 0) {
        printf("Erro ao enviar mensagem de jogada inválida\n");
    }

    exibe_jogadas(msg, socket_cliente);
}

void nomesJogadasCliente(GameMessage *msg, char *nome){
    switch(msg->client_action){
        case 0:
            strcpy(nome, "Nuclear Attack");
            break;

        case 1:
            strcpy(nome, "Intercept Attack");
            break;
        
        case 2:
            strcpy(nome, "Cyber Attack");
            break;  
        
        case 3:
            strcpy(nome, "Drone Strike");
            break;  

        case 4:
            strcpy(nome, "Bio Attack");
            break;  
    }
}

void nomesJogadasServidor(GameMessage *msg, char *nome){
    switch(msg->server_action){
        case 0:
            strcpy(nome, "Nuclear Attack");
            break;

        case 1:
            strcpy(nome, "Intercept Attack");
            break;
        
        case 2:
            strcpy(nome, "Cyber Attack");
            break;  
        
        case 3:
            strcpy(nome, "Drone Strike");
            break;  

        case 4:
            strcpy(nome, "Bio Attack");
            break;
    }
}

void resultadoRodada(int socket, GameMessage *msg){
    msg->type = MSG_RESULT;

    char nomeCliente[20];
    char nomeServidor[20];
    char resultado[20];

    nomesJogadasCliente(msg, nomeCliente);
    nomesJogadasServidor(msg, nomeServidor);
    calculoResultado(msg, resultado);

    snprintf(msg->message, MSG_SIZE,
        "Voce escolheu: %s\n"
        "Servidor escolheu: %s\n"
        "Resultado: %s\n",
        nomeCliente,
        nomeServidor,
        resultado
    );

    if (send(socket, msg, sizeof(GameMessage), 0) < 0) {
        printf("Erro ao enviar mensagem de resultado da rodada\n");
    }

}

void recebeMensagem(int socket, GameMessage *msg){
    if (recv(socket, msg, sizeof(GameMessage), 0) < 0) {
        printf("Erro ao receber mensagem.\n");
    }

    switch(msg->type) {

        case MSG_REQUEST:
            printf("%s\n", msg->message);
            enviaEscolha(socket, msg);
            break;


        case MSG_RESPONSE:
        printf("Cliente escolheu %d.\n", msg->client_action);

        if (msg->client_action < 0 || msg->client_action > 4) {
            printf("Erro: opção inválida de jogada.\n");
            jogadaInvalida(msg, &socket);
        } else {
            msg->server_action = escolha_aleatoria();
            printf("Servidor escolheu aleatoriamente %d.\n", msg->server_action);
            determina_resultado(msg);
            calculaHistorico(msg);
            resultadoRodada(socket, msg);

            if (msg->client_action == msg->server_action) {
                exibe_jogadas(msg, &socket);
            }
            else {
                jogarNovamente(&socket, msg);
            }
        }
            break;


        case MSG_RESULT:
            printf("%s\n", msg->message);
            break;


        case MSG_PLAY_AGAIN_REQUEST:
            printf("%s\n", msg->message);
            enviaJogarNovamente(socket, msg);
            break;


        case MSG_PLAY_AGAIN_RESPONSE:
            if (msg->result == 1){
                exibe_jogadas(msg, &socket);
            }
            else if (msg->result == 0) {
                printf("Cliente não deseja jogar novamente.\n");
                printf("Enviando placar final.\n");
                printf("Encerrando conexão.\n");
                printf("Cliente desconectado.\n");
                encerrando_conexao(msg, socket);
            }
            else {
                printf("Erro: resposta inválida para jogar novamente.\n");
                invalidaJogarNovamente(msg, socket);
            }
            break;


        case MSG_ERROR:
            printf("%s\n", msg->message);
            break;


        case MSG_END:
            printf("%s\n", msg->message);
            break;

    }
}
