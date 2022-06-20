#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <stdbool.h>
int main(int argc, char const *argv[])
{
	struct sockaddr_in serv_addr, client_address;
	int client_address_size, port_number, socket_fd, newsocket_fd, flag_1;
	char buffer[3], currency[3], port[10];
	float rate, result;
	bool flag = false;
	FILE *fp;
	char *line = NULL;
	size_t len = 0;
	ssize_t readline;
	
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd  < 0) {
		perror("Error in creating the socket\n");
		return (-1);
	}
	bzero((char *)&serv_addr, sizeof(serv_addr));
	printf("Enter the server port number: ");
	fflush(stdout);
	fgets(port, sizeof(port), stdin);
	port[strlen(port) - 1] = '\0';
	port_number = atoi(port);
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port_number);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	
	if (bind(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) //Binding socket to the address
		perror("ERROR on binding\n");
	listen(socket_fd, 5);                           //Listen on the socket for connection
	client_address_size = sizeof(client_address);
	while (1)
	{
		newsocket_fd = accept(socket_fd, (struct sockaddr *)&client_address, (socklen_t *)&client_address_size); // accept() a client to connect to server
		if (newsocket_fd < 0)
			perror("ERROR on accept()\n");
		flag_1 = read(newsocket_fd, buffer, 3);
		if (buffer[0] != '?' && buffer[1] != '?' && buffer[2] != '?')	// if the currency code is not "???" (terminate)
		{   
			printf("\nClient requested rate for %3s\n", buffer);
		}
		fp = fopen("./rates20.txt", "r");
		if (fp == NULL)
			exit(EXIT_FAILURE);
		while ((readline = getline(&line, &len, fp)) != -1)
		{
			sscanf(line, "%3s %f", currency, &rate);
			if (currency[0] == buffer[0] && currency[1] == buffer[1] && currency[2] == buffer[2])
			{
				flag = true;
				break;
			}
		}
		fclose(fp);
		if (flag)    // The currency code is found in the database
		{
			result = rate;
			printf("Today’s rate is: %f\n",rate);
		}
		else
		{	
			if (buffer[0] == '?' && buffer[1] == '?' && buffer[2] == '?')  // if terminate command "???" was inserted!
			{	
				printf("\nClient requested termination\n");
				close(newsocket_fd); 
				close(socket_fd);
				return 0;
			}
			else
			{                         // The currency code is not found in the database!
				result = -1;
				printf("Today’s rate is: UNKNOWN\n");
			}
		}
		flag_1 = write(newsocket_fd, &result, sizeof(result));
		flag = false;
		close(newsocket_fd);
	}close(socket_fd);
}	