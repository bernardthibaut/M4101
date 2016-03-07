#include <stdio.h>
#include <string.h>
#include "socket.h"
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <signal.h>


int words(char * ligne)
{
	int counted = 0;
	const char* it = ligne;
	int inword = 0;

	do switch(*it) 
	{
		case '\0': 
		case ' ': case '\t': case '\n': case '\r':
		if (inword) { inword = 0; counted++; }
		break;
		default: inword = 1;
	} 
	while(*it++);

	return counted;
}

int checkPremiereLigne(char* premiereLigne)
{
	char * buffer = malloc(4 * sizeof(char));
	int i;
	for (i = 0; i < 4; ++i)
	{
		buffer[i] = premiereLigne[i];
	}

	if(strcmp(buffer, "GET ") != 0) 
	{
		return -1;
	}

	if(words(premiereLigne) != 3)
	{
		return -1;
	}

	char * troisiemeMot = strstr(premiereLigne, "HTTP/");
	int position = troisiemeMot - premiereLigne;

	buffer = realloc(buffer, 8 * sizeof(char));
	for(i = 0; i < 8; i++) 
	{
		buffer[i] = premiereLigne[i + position];
	}

	if(strcmp(buffer, "HTTP/1.0") != 0 && strcmp(buffer, "HTTP/1.1") != 0)
	{
		return -1;
	}

	return 0;
}

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

			int taille = 256 * sizeof(char);
			char * buffer = malloc(taille);
			FILE * socket_client_file = fdopen(socket_client, "w+");
			fgets(buffer, taille, socket_client_file);

			while(strcmp(buffer, "\n") == 0)
			{
				write(socket_client, "Chaine vide, try again\n", strlen("Chaine vide, try again\n"));
				fgets(buffer, taille, socket_client_file);
			}

			if(checkPremiereLigne(buffer) != 0) {
				write(socket_client, "HTTP/1.1 400 Bad Request\r\n", strlen("HTTP/1.1 400 Bad Request\r\n"));
				write(socket_client, "Connection: close\r\n", strlen("Connection: close\r\n"));
				write(socket_client, "Content-Length: 17\r\n", strlen("Content-Length: 17\r\n"));
				write(socket_client, "\r\n", strlen("\r\n"));
				write(socket_client, "400 Bad request\r\n", strlen("400 Bad request\r\n"));
				return -1;
			}

			write(socket_client, "HTTP/1.1 200 OK\r\n", strlen("HTTP/1.1 200 OK\r\n"));
			sleep(1);
			int i;
			for(i = 0; i<10; i++)
			{
				write(socket_client, message_bienvenue, strlen(message_bienvenue));
			}

			printf("<%d> %s", getpid(), buffer);
			while(fgets(buffer, taille, socket_client_file) != NULL) {
				printf("<%d> %s", getpid(), buffer);
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