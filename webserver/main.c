#include <stdio.h>
#include <string.h>
#include "socket.h"
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <signal.h>

int main(void)
{

	printf("%s\n", "Lancement de Swanson");

	int socket_serveur = creer_serveur(8080);

	while(1)
	{

		int socket_client;
		socket_client = accept(socket_serveur, NULL ,NULL);
		if (socket_client == -1)
		{
			perror("accept socket_serveur");
			return -1;
		}


		const char * message_bienvenue = "She's here. I can smell it\n" ;

		if(fork() == 0)
		{
			sleep(1);
			int i;
			for(i = 0; i<10; i++)
			{
				write(socket_client, message_bienvenue, strlen(message_bienvenue));
			}

			int client_ecoute = 1;
			while(client_ecoute == 1)
			{
				char * buffer = malloc(256);
				if(read(socket_client, buffer, sizeof(buffer)) == -1)
				{
					perror("read");
					return -1;
				}
				if(strncmp("/exit", buffer, 5) == 0)
				{
					client_ecoute = 0;
					if(close(socket_client) == -1)
					{
						perror("close socket_client");
						return -1;
					}
				}
				else {
					write(socket_client, buffer, sizeof(buffer));
				}
			}
		} 
		else
		{
			if(close(socket_client) == -1)
			{
				perror("close socket_client");
				return -1;
			}
		}
	}

	if(close(socket_serveur) == -1)
	{
		perror("close socket_serveur");
		return -1;
	}

	return 0;
}
