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

			int taille = 256 * sizeof(char);
			char * buffer = malloc(taille);
			FILE * socket_client_file = fdopen(socket_client, "w+");
			while(fgets(buffer, taille, socket_client_file) != '\0') {
				fprintf(socket_client_file, "%s %s", "<Swanson>", buffer);
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
