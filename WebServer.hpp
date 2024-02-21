/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/25 13:25:40 by hoigag            #+#    #+#             */
/*   Updated: 2024/02/15 10:38:13 by hoigag           ###   ########.fr       */
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
// #define SERVER_PORT 3030
#define REQUEST_LENGTH 1024
#include <map>
class WebServer
{
	public:
		WebServer(ConfigData server);
		~WebServer();
		void createSocket();
		void listenForConnections();
		void bindSocket();
		void sendResponse(Request req, int sock);
		std::string directoryListing(std::string& path);
		
		// WebServer(const WebServer& other);
		// WebServer& operator=(const WebServer& other);
	private:
		std::map<int, std::string> clients;
		ConfigData server;
		int listenFD;
		struct sockaddr_in servaddr;
		Mimes mimes;
};

#endif
