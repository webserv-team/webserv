/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/25 13:25:40 by hoigag            #+#    #+#             */
/*   Updated: 2024/03/04 18:26:02 by hoigag           ###   ########.fr       */
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
#include "parsing/ServerConf.hpp"
#include "Mimes.hpp"
#include "Response.hpp"
#include "Socket.hpp"
// #define SERVER_PORT 3030
#define REQUEST_LENGTH 1024
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
};

struct ClientResponse
{
	Response response;
	int dataSent;
	int totalDataSent;
	int responseSize;
	bool isResponseFinished;
};

class WebServer
{
	public:
		WebServer(std::vector<Socket>& httpServers);
		WebServer(ConfigData server);
		~WebServer();
		void createSocket();
		void listenForConnections();
		void bindSocket();
		Response formResponse(Request req);
		std::string directoryListing(std::string& path);
		
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
