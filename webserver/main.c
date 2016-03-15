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

int checkGet(char* ligne)
{
	char * buffer = malloc(4 * sizeof(char));
	int i;
	for (i = 0; i < 4; ++i)
	{
		buffer[i] = ligne[i];
	}

	if(strcmp(buffer, "GET ") != 0) 
	{
		return -1;
	}

	if(words(ligne) != 3)
	{
		return -1;
	}

	char * troisiemeMot = strstr(ligne, "HTTP/");
	int position = troisiemeMot - ligne;

	buffer = realloc(buffer, 8 * sizeof(char));
	for(i = 0; i < 8; i++) 
	{
		buffer[i] = ligne[i + position];
	}

	if(strcmp(buffer, "HTTP/1.0") != 0 && strcmp(buffer, "HTTP/1.1") != 0)
	{
		return -1;
	}

	return 0;
}

int checkHost(char* ligne) {
	char * buffer = malloc(6 * sizeof(char));
	int i;
	for (i = 0; i < 6; ++i)
	{
		buffer[i] = ligne[i];
	}

	if(strcmp(buffer, "Host: ") != 0) 
	{
		return -1;
	}

	if(words(ligne) != 2)
	{
		return -1;
	}

	return 0;
}

int checkURL(char * ligne){
	char * buffer = malloc(6 * sizeof(char));
	int i;
	for (i = 0; i < 6; ++i)
	{
		buffer[i] = ligne[i];
	}

	if(strcmp(buffer, "GET / ") != 0) 
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

			int lignesPresentes = 0;
			int wrongURL = 0;

			while(strcmp(buffer, "\n") != 0)
			{
				if(lignesPresentes == 0 && checkGet(buffer) == 0){
					if(checkURL(buffer) != 0){
						wrongURL = 1;
					}
					lignesPresentes++;
				}
				if(lignesPresentes == 1 && checkHost(buffer) == 0){
					lignesPresentes++;
				}
				fgets(buffer, taille, socket_client_file);
			}

			if(lignesPresentes != 2) {
				fprintf(socket_client_file, "HTTP/1.1 400 Bad Request\r\n");
				fprintf(socket_client_file, "Connection: close\r\n");
				fprintf(socket_client_file, "Content-Length: 17\r\n");
				fprintf(socket_client_file, "\r\n");
				fprintf(socket_client_file, "400 Bad request\r\n");
				return -1;
			}

			if(wrongURL != 0){
				fprintf(socket_client_file, "HTTP/1.1 404 URL Not Found\r\n");
				fprintf(socket_client_file, "Connection: close\r\n");
				fprintf(socket_client_file, "Content-Length: 17\r\n");
				fprintf(socket_client_file, "\r\n");
				fprintf(socket_client_file, "404 URL Not Found\r\n");
				return -1;
			}

			fprintf(socket_client_file, "HTTP/1.1 200 OK\r\n");
			sleep(1);
			int i;
			for(i = 0; i<10; i++)
			{
				fprintf(socket_client_file, message_bienvenue);
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