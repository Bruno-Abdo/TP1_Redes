#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFSZ 1024

void usage(int argc, char **argv) //Verifica a conexão do servidor
{
	printf("usage: %s <server IP> <server port>\n", argv[0]);
	printf("example: %s 127.0.0.1 51511\n", argv[0]);
	exit(EXIT_FAILURE);
}

int startConnection(int argc, char **argv, int s) //Implementa a conexão com o servidor socket() e connect()
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
	return s;
}

int main(int argc, char **argv)
{
	int s;				// Inicialização do Socket
	struct action Jogo; // Declara estrutura
	GameMessage Game;
	MessageType Message;
	char buf[BUFSZ];
	unsigned total = 0;
	size_t count = 0;
	int buffer = 0;
	int winning = 0;

	s = startConnection(argc, argv, s); // Faz socket(), bind(), listen() e accept()


	recv(s, &Game, sizeof(Game), 0); // Recebe 

	if(Game.type == 0){
		printf("Escolha a sua jogada:\n\n0 - Nuclear Attack\n1 - Intercept Attack\n2 - Cyber Attack\n3 - Drone Strike\n4 - Bio Attack\n");
		scanf("%d",&Game.client_action);
		count = send(s, &Game, sizeof(Game), 0); // Envia o movimento escolhido
				if (count != sizeof(Game))
				{
					logexit("send");
				}

		recv(s, &Game, sizeof(Game), 0); // Recebe os movimentos possíveis do jogador
		printf("%s\n",Game.message);

	}



	while (1)
	{

		recv(s, &Jogo, sizeof(Jogo), 0); // Recebe os movimentos possíveis do jogador


		while (1)
		{
		
			if (Jogo.type == 6)
			{
				count = send(s, &Jogo, sizeof(Jogo), 0); // Envia o movimento escolhido
				if (count != sizeof(Jogo))
				{
					logexit("send");
				}
				winning = 0;
				break;
			}

			if (Jogo.type == 1 && winning == 0)
			{
				if (Jogo.moves[0] == buffer || Jogo.moves[1] == buffer || Jogo.moves[2] == buffer || Jogo.moves[3] == buffer)
				{

					for (int y = 0; y < 100; y++)
					{
						Jogo.moves[y] = 0;
					}

					Jogo.moves[0] = buffer;
					count = send(s, &Jogo, sizeof(Jogo), 0); // Envia o movimento escolhido
					if (count != sizeof(Jogo))
					{
						logexit("send");
					}
					recv(s, &Jogo, sizeof(Jogo), 0); // Recebe os movimentos possíveis do jogador
					
				}
				else
					printf("error: you cannot go this way\n");
			}

			if (Jogo.type == 2 && winning == 0)
			{
				count = send(s, &Jogo, sizeof(Jogo), 0); // Envia o movimento escolhido
				if (count != sizeof(Jogo))
				{
					logexit("send");
				}

				recv(s, &Jogo, sizeof(Jogo), 0); // Recebe os movimentos possíveis do jogador
				
			}
			if (Jogo.type == 5 && winning == 0)
			{
				winning = 1;
				Jogo.type = 0;
				printf("You escaped!\n");
				
				printf("\n");
			}
		}
	}
}