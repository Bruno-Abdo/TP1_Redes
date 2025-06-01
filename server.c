#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <sys/socket.h>
#include <sys/types.h>

#define BUFSZ 1024

void usage(int argc, char **argv){
    printf("usage: %s <v4|v6> <server port>\n", argv[0]);
    printf("example: %s v4 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}

int startConnection(int argc, char **argv, char *straddr)
{

    if(argc < 3){
        usage(argc, argv);
    }

    struct sockaddr_storage storage;
    if(0 != server_sockaddr_init(argv[1], argv[2], &storage)){
        usage(argc, argv);
    }

    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if(s == -1){
        logexit("socket");
    }

    int enable = 1;
    if(0 != setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int))){
        logexit("setsockopt");
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if(0 != bind(s, addr, sizeof(storage))){
        logexit("bind");
    }
    
    printf("Servidor iniciado em modo %s na porta %s. Aguardando conexão...\n", argv[1],argv[2]);

    if(0 != listen(s, 10)){
        logexit("listen");
    }

    struct sockaddr_storage cstorage;
    struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
    socklen_t caddrlen = sizeof(cstorage);

    int sock = accept(s, caddr, &caddrlen);
    if (sock == -1)
    {
        logexit("accept");
    }
    printf("Cliente conectado.\n");

    char traddr[BUFSZ];
    addrtostr(caddr, straddr, BUFSZ);

    return sock;
}

int Random(){
    srand(time(NULL));
    return rand() % 6;
}

void GetResult(GameMessage *Jogo){

    if(Jogo->client_action == 0){
        if(Jogo->client_action == Jogo->server_action){
            Jogo->result = -1;
            strcpy(Jogo->message,"Empate!");
        }else if(Jogo->server_action == 2 || Jogo->server_action == 3){
            Jogo->result = 1;
            strcpy(Jogo->message,"Vitoria!");
            Jogo->client_wins++;
        } else{
            Jogo->result = 0;
            strcpy(Jogo->message,"Derrota!");
            Jogo->server_wins++;
        }

    } else if(Jogo->client_action == 1){
        if(Jogo->client_action == Jogo->server_action){
            Jogo->result = -1;
            strcpy(Jogo->message,"Empate!");
        }else if(Jogo->server_action == 0 || Jogo->server_action == 4){
            Jogo->result = 1;
            strcpy(Jogo->message,"Vitoria!");
            Jogo->client_wins++;
        } else{
            Jogo->result = 0;
            strcpy(Jogo->message,"Derrota!");
            Jogo->server_wins++;
        }

    } else if(Jogo->client_action == 2){
        if(Jogo->client_action == Jogo->server_action){
            Jogo->result = -1;
            strcpy(Jogo->message,"Empate!");
        }else if(Jogo->server_action == 1 || Jogo->server_action == 3){
            Jogo->result = 1;
            strcpy(Jogo->message,"Vitoria!");
            Jogo->client_wins++;
        } else{
            Jogo->result = 0;
            strcpy(Jogo->message,"Derrota!");
            Jogo->server_wins++;
        }

    } else if(Jogo->client_action == 3){
        if(Jogo->client_action == Jogo->server_action){
            Jogo->result = -1;
            strcpy(Jogo->message,"Empate!");
        }else if(Jogo->server_action == 1 || Jogo->server_action == 4){
            Jogo->result = 1;
            strcpy(Jogo->message,"Vitoria!");
            Jogo->client_wins++;
        } else{
            Jogo->result = 0;
            strcpy(Jogo->message,"Derrota!");
            Jogo->server_wins++;
        }

    } else if(Jogo->client_action == 4){
        if(Jogo->client_action == Jogo->server_action){
            Jogo->result = -1;
            strcpy(Jogo->message,"Empate!");
        }else if(Jogo->server_action == 2 || Jogo->server_action == 0){
            Jogo->result = 1;
            strcpy(Jogo->message,"Vitoria!");
            Jogo->client_wins++;
        } else{
            Jogo->result = 0;
            strcpy(Jogo->message,"Derrota!");
            Jogo->server_wins++;
        }
    }

    printf("Placar atualizado: Cliente %d x %d Servidor\n",Jogo->client_wins,Jogo->server_wins);
}

int main(int argc, char **argv){

    struct action Jogo;
    GameMessage Game;
	MessageType Message;
    int csock;
    char caddrstr[BUFSZ];
    size_t count = 0;
    
    csock = startConnection(argc, argv, caddrstr);//Conecta ao cliente

    Game.client_wins = 0;
    Game.server_wins = 0;

    while(1){
        if(Game.type == MSG_REQUEST){ //Para solicitar que o cliente envie uma jogada
            printf("Apresentando as opções para o cliente.\n");

            count = send(csock, &Game, sizeof(Game), 0);
            if (count != sizeof(Game)){
                logexit("send");
            }
        }

        if(Game.type == MSG_RESPONSE){ //Para enviar a jogada escolhida ao servidor
            printf("Cliente escolheu %d.\n",Game.client_action);

            if(Game.client_action < 0 || Game.client_action > 4){
                printf("Erro: opção inválida de jogada.");
                
                Game.type = MSG_ERROR;  //Quando a jogada do cliente for inválida
                strcpy(Game.message, "Por favor, selecione um valor de 0 a 4.");
                count = send(csock, &Game, sizeof(Game), 0);
                if (count != sizeof(Game)){
                    logexit("send");
                }
                Game.type = MSG_REQUEST;
                
            }else{
                Game.result = 5;
                GetResult(&Game);
                Game.type = MSG_RESULT;
                count = send(csock, &Game, sizeof(Game), 0);
                if (count != sizeof(Game)){
                    logexit("send");
                }
            }
        }
            

















                count = send(csock, &Game, sizeof(Game), 0);
                if (count != sizeof(Game))
                {
                    logexit("send");
                }
            }
        }
        Game.server_action = Random();
        printf("Servidor escolheu aleatoriamente %d.\n",Game.server_action);

        Game.result = 5;
        GetResult(&Game);

        Game.type = MSG_RESULT;

        count = send(csock, &Game, sizeof(Game), 0); 
        if (count != sizeof(Game))
        {
            logexit("send");
        }

        if(Game.result != -1){
            break;
        }

        
            count = recv(csock,&Game,sizeof(Game),0); //Recebe Mensagens
            if(count != sizeof(Game)){
                logexit("recv");
            }
    }

    while(1){
        Game.type = MSG_PLAY_AGAIN_REQUEST;
        count = send(csock, &Game, sizeof(Game), 0); 
            if (count != sizeof(Game))
            {
                logexit("send");
            }
        count = recv(csock,&Game,sizeof(Game),0); //Recebe Mensagens
        if(count != sizeof(Game)){
            logexit("recv");
        }

        if(Game.client_action == 0 || Game.client_action == 1){
            
        }
    }

    if(Game.client_action == 0){
        break;  
    }

Game.type = MSG_END;
count = send(csock, &Game, sizeof(Game), 0); 
    if (count != sizeof(Game))
    {
        logexit("send");
    }

    printf("Cliente não deseja jogar novamente.\nEnviando placar final.\nEncerrando conexão.\n");
    exit(EXIT_SUCCESS);
    printf("Cliente desconectado.\n");
}