/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hassan <hassan@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/25 13:25:40 by hoigag            #+#    #+#             */
/*   Updated: 2024/03/08 21:27:19 by hassan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>
#include "Cgi.hpp"
#include <sys/stat.h>
#include "ServerConf.hpp"
#include "Mimes.hpp"
#include "Response.hpp"
#include "Socket.hpp"
#include "Header.hpp"

#include <map>
struct Client
{
    std::string request;
    bool isRequestFinished;
	int bytesRead;
	int contentlength;
	std::string header;
	std::string content;
	int isHeaderFinished;
	Header headerObject;
};

struct ClientResponse
{
	Response response;
	int totalDataSent;
	int responseSize;
	bool isResponseFinished;
};

class WebServer
{
	public:
		WebServer(std::vector<Socket>& httpServers);
		~WebServer();
		void listenForConnections();
		Response formResponse(Request& req);
		Socket getServer(int port);
		// std::string directoryListing(std::string& path);
		bool isServerFd(int fd);
		void handleNewConnection(int fd);
		void handleExistingConnection(int fd);
		// WebServer(const WebServer& other);
		// WebServer& operator=(const WebServer& other);
	private:
		std::map<int, Client> clients;
		std::map<int, ClientResponse> clientResponses;
		int maxFd;
		fd_set read_sockets, read_copy_sockets;
    	fd_set write_sockets, write_copy_sockets;
		std::vector<Socket> httpServers;
		Mimes mimes;
};

#endif
