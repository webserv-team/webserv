/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/25 13:25:34 by hoigag            #+#    #+#             */
/*   Updated: 2024/01/06 12:34:21 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include <iostream>
#include <fstream>

WebServer::WebServer()
{
    this->port = 3030;
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

void WebServer::listenForConnections()
{
    int connFd;
    std::string html = loadFile("index.html");
    if ((listen(this->listenFD, 3)) < 0)
        std::runtime_error("error happened during listening for requests");
    while (true)
    {
        struct sockaddr_in addr;
        socklen_t addr_len;
        // std::cout << "listening on port " << this->port << std::endl;
        connFd = accept(this->listenFD,  (struct sockaddr *)&addr, (socklen_t *) &addr_len);
        if (connFd < 0)
        {
            std::cout << "could not accept the connection" << std::endl;
            exit(1);
        }
        std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ";
        response += std::to_string(html.length()) + "\n\n";
        response += html;
        char request[2048];
        int n = read(connFd, request, 2048);
        // calss test(request);
        if (n  < 0)
        {
            std::cout << "could not read the request" << std::endl;
            exit (1);
        }
        request[n] = '\0';
        //  this.parseRequest();
        // if (!got[0])
            // std::cout << "no message was sent" << std::endl;
        std::cout << request << std::endl;
        if (send(connFd, response.c_str(), response.length(), 0) < 0)
        {
            std::cout << "could not send message to client" << std::endl;
            exit(1);
        }
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

