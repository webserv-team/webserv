/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ogorfti <ogorfti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/02 15:53:30 by ogorfti           #+#    #+#             */
/*   Updated: 2023/11/07 15:41:36 by ogorfti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <list>

#define BUFFER_SIZE 4096
#define PORT 80

void	sendResponse(int sock)
{
	std::ifstream file("htdocs/index.html");
	std::stringstream fileContent;
	fileContent << file.rdbuf();
	std::string content = fileContent.str();
	file.close();
	
	std::string response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: text/html\r\n";
    response += "Content-Length: " + std::to_string(content.length()) + "\r\n";
    response += "\r\n";
    response += content;
	
	if (send(sock, response.c_str(), strlen(response.c_str()), 0) < 0)
	{
		std::cerr << "error: Could not send data" << std::endl;
		exit(1);		
	}
}

int main(void)
{
	//* Create a socket
	int serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (serverFd == -1)
	{
		std::cerr << "Socket creation failed" << '\n';
		exit (1);
	}

	//* Set up the server address structure
	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET; //ipv4
	serverAddress.sin_port = htons(PORT);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	//* Bind the socket to the server address
	if (bind(serverFd, (sockaddr*)&serverAddress, sizeof(serverAddress)) == -1)
	{
		std::cerr << "error: socket creation" << '\n';
		exit (1);	
	}

	//* Listen for incoming connections
	if (listen(serverFd, 3) == -1)
	{
		std::cerr << "error: listern call" << '\n';
		exit (1);	
	}
	
	socklen_t addrlen = sizeof(serverAddress);
	int sock, n;
	char buffer[BUFFER_SIZE];
	while (true)
	{
		printf("\n+++++++ Waiting for new connection ++++++++\n\n");
		sock = accept(serverFd, (struct sockaddr *)&serverAddress, &addrlen);
		if (sock < 0)
		{
			std::cerr << "error: accept call" << std::endl;
			exit(1);
		}

		if ((n = recv(sock, buffer, BUFFER_SIZE, 0)) < 0)
		{
			std::cerr << "error: Could not receive data" << std::endl;
			exit(1);			
		}
		buffer[n] = '\0';
		std::cout << buffer << '\n';

		sendResponse(sock);
		std::cout << "\033[1;32mSEND IT\033[0m\n";
		close (sock);
	}
	close (serverFd);
} 
