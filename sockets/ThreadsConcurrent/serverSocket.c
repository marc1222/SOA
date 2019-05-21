#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <pthread.h>

int *work_threads;

struct thread_arg{
	int thread_id;
	int fd;
};

void *doService(void *arguments) {
	struct thread_arg *args = (struct thread_arg *)arguments;
	int i = 0;
	char buff[80];
	char buff2[80];
	int ret;
	int socket_fd = (int) args->fd;

	ret = read(socket_fd, buff, sizeof(buff));
	while(ret > 0) {
		buff[ret]='\0';
		//sprintf(buff2, "Server [%d] received: %s\n", getpid(), buff);
		//write(1, buff2, strlen(buff2));
		ret = write(args->fd, "caracola ", 8);
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
	work_threads[args->thread_id] = 0;
}

int findEmptyThread(Max) {
	for (int i = 0; i < Max; ++i) {
		if (work_threads[i] == 0) return i;
	}
	return -1;
}

void print_work(Max) {
	char buff[20];
write(1, "----------------\n", 15);
	for (int i = 0; i < Max; ++i) {
		sprintf(buff, "Thread %d state %d\n", i, work_threads[i]);
			write(1, buff, strlen(buff));	
	}
write(1, "----------------\n", 15);
}

main (int argc, char *argv[])
{
	int socketFD;
	char buffer[80];
	int ret;
	int port, MaxConnections, connectionFD;

	if (argc != 3)
	{
		strcpy (buffer, "Usage: ServerSocket PortNumber MaxConcurrentConnections\n");
		write (2, buffer, strlen (buffer));
		exit (1);
	}

	port = atoi(argv[1]);
	MaxConnections = atoi(argv[2]);
	
	pthread_t threads[MaxConnections];
	work_threads = malloc(sizeof(int) * MaxConnections);
	for (int i = 0; i < MaxConnections; ++i) {
		work_threads[i] = 0;
	}
	int thread_id;
	struct thread_arg args[MaxConnections];
	socketFD = createServerSocket (port);
	if (socketFD < 0)
	{
		perror ("Error creating socket\n");
		exit (1);
	}

	while (1) {
		thread_id = findEmptyThread(MaxConnections);
		if (thread_id != -1) {
			connectionFD = acceptNewConnections (socketFD);
			if (connectionFD < 0)
			{
			  perror ("Error establishing connection \n");
			  deleteSocket(socketFD);
			  exit (1);
			}
			args[thread_id].thread_id = thread_id;
			args[thread_id].fd = connectionFD;
			work_threads[thread_id] = 1;
			pthread_create(&threads[thread_id], NULL, doService, &args[thread_id]);
			sprintf(buffer, "Thread id %d\n", thread_id);
			write(1, buffer, strlen(buffer));
				
		}
	}
}
