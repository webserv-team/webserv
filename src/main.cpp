/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ogorfti <ogorfti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 10:16:59 by ogorfti           #+#    #+#             */
/*   Updated: 2024/02/12 14:25:17 by ogorfti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../response/Response.hpp"

// socket -> bind -> listen -> accept -> recv -> send -> close
// socket is like a virtual divice likaysam7 ljuj computers ycommunicatiw m3a ba3dyatem
int main()
{
	int server_fd;

	// socket(domain, type, protocol) : domain = AF_INET (IPv4), type = SOCK_STREAM (TCP), protocol = 0 (IP)
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		cerr << "Socket creation error: " << strerror(errno) << endl;
		return (-1);
	}
	
	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		cerr << "Setsockopt error: " << strerror(errno) << endl;
		return (-1);
	}
	struct sockaddr_in address;

	address.sin_family = AF_INET; // IPv4
	address.sin_addr.s_addr = htonl(INADDR_ANY); // htonl() converts a long integer (e.g. address) to a network representation
	address.sin_port = htons(PORT); // htons() converts a short integer (e.g. port) to a network representation

	bind(server_fd, (struct sockaddr *)&address, sizeof(address));
	// if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	// {
	// 	cerr << "Bind error: " << strerror(errno) << endl;
	// 	return (-1);
	// }

	// listen(socket, backlog) : backlog = 3 (max number of pending connections)
	if (listen(server_fd, 3) < 0)
	{
		cerr << "Listen error: " << strerror(errno) << endl;
		return (-1);
	}

	socklen_t addrlen = sizeof(address);
	char buffer[BUFFER_SIZE];
	int new_socket, valread;
	
	ServerConf tmp("default.toml");
	vector<ConfigData> servers = tmp.getServers();
	signal(SIGPIPE, SIG_IGN); // this mean ignore the signal SIGPIPE because 
	while (true)
	{
		cout << "\n+++++++ Waiting for new connection ++++++++" << endl;
		new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
		if (new_socket < 0)
		{
			cerr << "Accept error: " << strerror(errno) << endl;
			return (-1);
		}
		if ((valread = recv(new_socket, buffer, BUFFER_SIZE, 0)) < 0)
		{
			cerr << "Recv error: " << strerror(errno) << endl;
			return (-1);
		}
		buffer[valread] = '\0';
		cout << "Request received: " << buffer << endl;
	   
		Request request(buffer);

		for (size_t i = 0; i < servers.size(); i++)
		{
			// if (servers[i].port == 8080)
			// {
				// cout << "Server found: " << servers[i].hostName << endl;
				// sendResponse(new_socket, request, servers[i]);
				Response::sendResponse(new_socket, request, servers[i]);
				// break;
			// }
		}
		close(new_socket);
	}
	close(server_fd);
}
