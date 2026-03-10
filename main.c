#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define MAX_PATH 4096
#define PORT 8086

size_t get_file_length(char *filename)
{
	FILE *fptr = fopen(filename, "r");
	if (fptr == NULL)
	{
		return 0;
	}

	size_t length = 0;

	while (fgetc(fptr) != EOF)
	{
		length++;
	}

	fclose(fptr);

	return length;
}

void send_file(int connection, char *path)
{
	char fpath[MAX_PATH];
	snprintf(fpath, MAX_PATH, "./static%s", path);
	char *file_path = realpath(fpath, NULL);
	if (file_path == NULL) return;

	char *cwd = realpath("./static", NULL);
	if (strncmp(cwd, file_path, strlen(cwd)) != 0) return; // if the path doesn't begin with the static directory then it might be a path traversal
	free(cwd);

	size_t content_length = get_file_length(file_path);

	char buffer[4096];
	snprintf(buffer, 4096, "HTTP/1.1 200 OK\r\nContent-Length: %zu\r\nContent-Type: text/html\r\n\r\n", content_length);

	send(connection, buffer, strlen(buffer), 0);

	FILE *fptr = fopen(file_path, "r");
	free(file_path);
	if (fptr == NULL)
	{
		return;
	}

	char *file_data = malloc(content_length);
	if (file_data == NULL)
	{
		fclose(fptr);
		return;
	}

	fread(file_data, sizeof(char), content_length, fptr);

	send(connection, file_data, content_length, 0);
	free(file_data);
	file_data = NULL;

	fclose(fptr);
}

void *handle_connection(void *connection)
{
	char buffer[1024];
	int bytes = recv(*((int *)connection), buffer, 1024, 0);
	char method[7], path[257], protocol[9];
		
	sscanf(buffer, "%6s %256s %8s", method, path, protocol);
	send_file(*((int *)connection), path);

	close(*((int *)connection));
	free(connection);
	return NULL;
}

int main(void)
{
	int serv_fd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in sockaddr;
	socklen_t socklen = sizeof(sockaddr);
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(PORT);
	sockaddr.sin_addr.s_addr = INADDR_ANY;

	int bind_result = bind(serv_fd, (struct sockaddr *)&sockaddr, socklen);
	if (bind_result < 0)
	{
		printf("Could't bind to port. Make sure the port isn't in use.\n");
		return EXIT_FAILURE;
	}

	int listen_result = listen(serv_fd, 128);
	if (listen_result < 0)
	{
		printf("Couldn't start listening.\n");
		return EXIT_FAILURE;
	}

	while(true)
	{
		int connection = accept(serv_fd, (struct sockaddr *)&sockaddr, &socklen);
		
		pthread_t thread;
		int *arg = malloc(sizeof(int));
		*arg = connection;
		pthread_create(&thread, NULL, handle_connection, arg);
		pthread_detach(thread);
	}

	close(serv_fd);

	return EXIT_SUCCESS;
}

