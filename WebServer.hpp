/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/25 13:25:40 by hoigag            #+#    #+#             */
/*   Updated: 2024/01/16 16:54:08 by hoigag           ###   ########.fr       */
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
#include "parsing/Request.hpp"
#define SERVER_PORT 3030
#define REQUEST_LENGTH 1024

class WebServer
{
	public:
		WebServer();
		~WebServer();
		void createSocket();
		void listenForConnections();
		void bindSocket();
		// WebServer(const WebServer& other);
		// WebServer& operator=(const WebServer& other);
	private:
		short port;
		int ip;
		int listenFD;
		struct sockaddr_in servaddr;
};

#endif
