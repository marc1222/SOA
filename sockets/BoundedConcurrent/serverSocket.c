#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>

int Connections = 0;

void trat_sigchld(int signal) {
	while (waitpid(-1, NULL, WNOHANG) > 0) Connections--;
}

doService(int fd) {
int i = 0;
char buff[80];
char buff2[80];
int ret;
int socket_fd = (int) fd;

	ret = read(socket_fd, buff, sizeof(buff));
	while(ret > 0) {
		buff[ret]='\0';
		//sprintf(buff2, "Server [%d] received: %s\n", getpid(), buff);
		//write(1, buff2, strlen(buff2));
		ret = write(fd, "caracola ", 8);
		if (ret < 0) {
			perror ("Error writing to socket");
			exit(1);
		}
		ret = read(socket_fd, buff, sizeof(buff));
	}
	if (ret < 0) {
			perror ("Error reading from socket");

	}
	//sprintf(buff2, "Server [%d] ends service\n", getpid());
	//write(1, buff2, strlen(buff2));

}

doServiceFork(int fd){
	int n;
	if(fork() == 0) {
		doService(fd);
		return 0;
	}
}


main (int argc, char *argv[])
{
	int socketFD;
	int connectionFD;
	char buffer[80];
	int ret;
	int port, MaxConnections;


	if (argc != 3)
	{
		strcpy (buffer, "Usage: ServerSocket PortNumber MaxConcurrentConnections\n");
		write (2, buffer, strlen (buffer));
		exit (1);
	}

	port = atoi(argv[1]);
	MaxConnections = atoi(argv[2]);

	signal(SIGCHLD,trat_sigchld);

	socketFD = createServerSocket (port);
	if (socketFD < 0)
	{
		perror ("Error creating socket\n");
		exit (1);
	}

	while (1) {
		while(Connections == MaxConnections);

		connectionFD = acceptNewConnections (socketFD);
		if (connectionFD < 0)
		{
		  perror ("Error establishing connection \n");
		  deleteSocket(socketFD);
		  exit (1);
		}
		Connections++;
		doServiceFork(connectionFD);
		sprintf(buffer, "Connections: %d\n", Connections);
		write(1, buffer, strlen(buffer));	
	}
}
