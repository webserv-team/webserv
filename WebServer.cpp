/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/25 13:25:34 by hoigag            #+#    #+#             */
/*   Updated: 2024/01/16 17:55:20 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include <iostream>
#include <fstream>

std::string loadFile(std::string file)
{
    std::ifstream inFile(file);
    if (!inFile.is_open())
    {
        std::cerr << "could not open file" << std::endl;
        exit(1);
    }
    std::string outputString = "";
    while (inFile)
    {
        std::string line;
        std::getline(inFile, line);
        line += "\n";
        outputString += line;
    }
    return outputString;
}

void	sendResponse(Request req, int sock)
{
    std::string content = loadFile(req.getURL());
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
    close(sock);
}

WebServer::WebServer()
{
    this->port = SERVER_PORT;
    this->ip = 0x7f000001;
}

void WebServer::createSocket()
{
    this->listenFD = socket(AF_INET, SOCK_STREAM, 0);
    if (this->listenFD < 0)
        throw std::runtime_error("could not create server socket");
}

void WebServer::bindSocket()
{
    bzero(&this->servaddr, sizeof(this->servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(this->ip);
    servaddr.sin_port = htons(this->port);

    //*binding the socket to the server
    if(bind(this->listenFD, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
        throw std::runtime_error("could not bind to socket");
}


void WebServer::listenForConnections()
{
    int connFd;
    // std::string html = loadFile("index.html");
    if ((listen(this->listenFD, 3)) < 0)
        std::runtime_error("error happened during listening for requests");
    while (true)
    {
        struct sockaddr_in addr;
        socklen_t addr_len;
        std::cout << "listening on port " << this->port << std::endl;
        connFd = accept(this->listenFD,  (struct sockaddr *)&addr, (socklen_t *) &addr_len);
        if (connFd < 0)
        {
            std::cout << "could not accept the connection" << std::endl;
            exit(1);
        }
        char request[REQUEST_LENGTH];
        int n = read(connFd, request, REQUEST_LENGTH);
        if (n  < 0)
        {
            std::cout << "could not read the request" << std::endl;
            exit (1);
        }
        request[n] = '\0';
        std::cout << request << std::endl;
        Request req(request);
        sendResponse(req, connFd);
        close(connFd);
    }
}

WebServer::~WebServer()
{

}

// WebServer::WebServer(const WebServer& other)
// {

// }

// WebServer& WebServer::operator=(const WebServer& other)
// {

// }

