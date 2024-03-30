/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServersMonitor.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/25 13:25:40 by hoigag            #+#    #+#             */
/*   Updated: 2024/03/30 21:23:30 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERSMONITOR_HPP
#define SERVERSMONITOR_HPP

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>
#include <fstream>
#include <sys/stat.h>
#include <map>
#include "ServerConf.hpp"
#include "Mimes.hpp"
#include "HttpServer.hpp"
#include "Header.hpp"
#include "helpers.hpp"
#include "Cgi.hpp"
#include "Response.hpp"

#define GREEN "\033[1;32m"
#define RED "\033[1;31m"
#define RESET "\033[0m"

struct Client
{
    std::string request;
    bool isRequestFinished;
	int bytesRead;
	int contentlength;
	bool isBody;
	int isHeaderFinished;
	short port;
	Header headerObject;
};

struct ClientResponse
{
	std::string response;
	int totalDataSent;
	int responseSize;
	bool isResponseFinished;
};

class ServersMonitor
{
	public:
		ServersMonitor();
		ServersMonitor(std::vector<HttpServer>& httpServers);
		~ServersMonitor();
		void listenForConnections();
		Response formResponse(Request& req);
		HttpServer getServer(Request& req, short port);
		// std::string directoryListing(std::string& path);
		bool isServerFd(int fd);
		void handleNewConnection(int fd);
		void handleExistingConnection(int fd);
		// ServersMonitor(const ServersMonitor& other);
		// ServersMonitor& operator=(const ServersMonitor& other);
	private:
		std::map<int, Client> clients;
		std::map<int, ClientResponse> clientResponses;
		int maxFd;
		fd_set read_sockets, read_copy_sockets;
    	fd_set write_sockets, write_copy_sockets;
		std::vector<HttpServer> httpServers;
		Mimes mimes;
};

#endif
