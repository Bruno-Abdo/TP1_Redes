#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFSZ 1024

void usage(int argc, char **argv) // Verifica a conexão do servidor
{
	printf("usage: %s <server IP> <server port>\n", argv[0]);
	printf("example: %s 127.0.0.1 51511\n", argv[0]);
	exit(EXIT_FAILURE);
}

int startConnection(int argc, char **argv, int s) // Implementa a conexão com o servidor socket() e connect()
{
	if (argc < 3)
	{
		usage(argc, argv);
	}

	struct sockaddr_storage storage;
	if (0 != addrparse(argv[1], argv[2], &storage))
	{
		usage(argc, argv);
	}

	s = socket(storage.ss_family, SOCK_STREAM, 0);
	if (s == -1)
	{
		logexit("socket");
	}
	struct sockaddr *addr = (struct sockaddr *)(&storage);
	if (0 != connect(s, addr, sizeof(storage)))
	{
		logexit("connect");
	}
	printf("Conectado ao servidor.\n");

	return s;
}

int main(int argc, char **argv)
{
	int s;				// Inicialização do Socket
	struct action Jogo; // Declara estrutura
	GameMessage Game;
	MessageType Message;
	char buf[BUFSZ];
	size_t count = 0;

	char attacks[5][17];

	strcpy(attacks[0],"Nuclear Attack");
	strcpy(attacks[1],"Intercept Attack");
	strcpy(attacks[2],"Cyber Attack");
	strcpy(attacks[3],"Drone Strike");
	strcpy(attacks[4],"Bio Attack");

	s = startConnection(argc, argv, s); // Faz socket(), bind(), listen() e accept()

	recv(s, &Game, sizeof(Game), 0);

	while (1){
		if (Game.type == MSG_REQUEST){
			printf("Escolha a sua jogada:\n\n0 - Nuclear Attack\n1 - Intercept Attack\n2 - Cyber Attack\n3 - Drone Strike\n4 - Bio Attack\n");
			scanf("%d", &Game.client_action);
			Game.type = MSG_RESPONSE;
			count = send(s, &Game, sizeof(Game), 0); // Envia o movimento escolhido
			if (count != sizeof(Game))
			{
				logexit("send");
			}
		
			recv(s, &Game, sizeof(Game), 0);
			if(Game.type == MSG_ERROR){
				printf("%s\n",Game.message);
				recv(s, &Game, sizeof(Game), 0);
			}
		}

		if(Game.type == MSG_RESULT){
			printf("Você escolheu: %s\n Servidor escolheu: %s\n Resultado: %s\n", attacks[Game.client_action],attacks[Game.server_action],Game.message);
			recv(s, &Game, sizeof(Game), 0);
		}

		if (Game.type == MSG_PLAY_AGAIN_REQUEST)
		{
			printf("Deseja jogar novamente?\n1 - Sim\n0 - Não\n");
			scanf("%d", &Game.client_action);
			Game.type = MSG_PLAY_AGAIN_RESPONSE;
			count = send(s, &Game, sizeof(Game), 0); // Envia o movimento escolhido
			if (count != sizeof(Game))
			{
				logexit("send");
			}

			recv(s, &Game, sizeof(Game), 0);
			if(Game.type == MSG_ERROR){
				printf("%s",Game.message);
				recv(s, &Game, sizeof(Game), 0);
			}
		}

		if(Game.type == MSG_END){
			printf("%s", Game.message);
			break;
		}
	}
}