#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
void error(char const *error_msg){
	perror(error_msg);
	exit(0);
}
int Socket_establish(char const *servername, int port_number)
{
	int socket_fd; 
	struct sockaddr_in serv_addr;
	struct hostent *server;
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0)
		error("ERROR: Cannot open the socket");
	server = gethostbyname(servername);
	if (server == NULL)
	{
		fprintf(stderr, "ERROR: Invalid server hostname\n");
		exit(0);
	}
	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr_list[0], (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(port_number);
	if (connect(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR connecting");
	
	return socket_fd;
}

int main()
{
	struct hostent *server;
	char servername[256], port[10], currency[3];
	float buffer;
	int flag, port_number, socket_fd;
	
	printf("Enter the server host name: ");
	fflush(stdout);
	fgets(servername, sizeof(servername), stdin);
	servername[strlen(servername) - 1] = '\0';	
	server = gethostbyname(servername);
	while(server == NULL)
	{
		fprintf(stderr, "ERROR: Invalid server hostname\n");
		printf("Enter a valid server host name: ");
		fflush(stdout);
		fgets(servername, sizeof(servername), stdin);
		servername[strlen(servername) - 1] = '\0';
		server = gethostbyname(servername);
	}
	printf("Enter the server port number: ");
	fflush(stdout);
	fgets(port, sizeof(port), stdin);
	port[strlen(port) - 1] = '\0';
	port_number = atoi(port);

	printf("\nEnter a currency code (3 Uppercase characters): ");
	fflush(stdout);
	fgets(currency, 4, stdin);

	socket_fd = Socket_establish(servername, port_number);

	flag = write(socket_fd, currency, 3);
	if (flag < 0)
		error("ERROR: Cannot write to socket\n");
	
	if(strcmp(currency,"???") == 0)
	{
		printf("\nClient is terminated!\n");
		return 0;
	}
	flag = read(socket_fd, &buffer, sizeof(buffer));
	if (flag < 0)
		error("ERROR: Cannot read from socket\n");

	if (buffer != -1)
	{
		printf("Today one %s is worth %f USD\n\n", currency, buffer);
	}
	else{
		printf("Unknown currency\n");
	}
}

