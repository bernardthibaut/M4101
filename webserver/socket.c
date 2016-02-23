#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

void traitement_signal()
{
	waitpid(-1, NULL, WNOHANG);
}

void initialiser_signaux(void)
{

	if(signal(SIGPIPE, SIG_IGN) == SIG_ERR)
	{
		perror("signal");
	}

	struct sigaction sa;
	sa.sa_handler = traitement_signal;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL ) == -1)
	{
		perror("sigaction(SIGCHLD)");
	}
}

int creer_serveur(int port)
{

	initialiser_signaux();

	int socket_serveur = socket(AF_INET, SOCK_STREAM, 0);

	int option_value = 1;
	if(setsockopt(socket_serveur, SOL_SOCKET, SO_REUSEADDR, &option_value , sizeof(int)) == -1)
	{
		perror("Can not set SO_REUSEADDR option");
		return -1;
	}

	if(socket_serveur == -1)
	{
		perror("socket_serveur");
		return -1;
	}

	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons (port);
	saddr.sin_addr.s_addr = INADDR_ANY;
	if(bind(socket_serveur, (struct sockaddr *)&saddr, sizeof(saddr)) == -1)
	{
		perror("bind socket_serveur");
		return -1;
	}

	if(listen(socket_serveur, 10) == -1)
	{
		perror("listen socket_serveur");
		return -1;
	}

	return socket_serveur;
}
