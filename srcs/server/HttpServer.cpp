/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpServer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 17:54:34 by hoigag            #+#    #+#             */
/*   Updated: 2024/03/30 22:00:16 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpServer.hpp"

//default constructor
HttpServer::HttpServer()
{

}

HttpServer::HttpServer(ConfigData& conf)
{
    this->conf = conf;
}
void HttpServer::start(fd_set& read_sockets, int& maxFd)
{
    std::cout << PURPLE << "server " << this->conf.serverName << ": "<< this->conf.host << " listening on ports: ";
    std::vector<int> ports = this->conf.ports;
    
    for (size_t i = 0; i < ports.size(); i++)
    {
        Socket socket(ports[i], this->conf.host);
        socket.setPort(ports[i]);
        int portFd = socket.getFd();
        std::cout << ports[i] << " ";
        if (portFd > maxFd)
            maxFd = portFd;
        FD_SET(portFd, &read_sockets);
        this->portFds.push_back(socket);
    }
    std::cout << RESET << std::endl;
}


HttpServer::~HttpServer()
{

}

std::vector<Socket> HttpServer::getSockets()
{
    return this->portFds;
}

HttpServer::HttpServer(const HttpServer& other)
{
    *this = other;
}

HttpServer& HttpServer::operator=(const HttpServer& other)
{
    this->portFds = other.portFds;
    this->conf = other.conf;
    return *this;
}

ConfigData HttpServer::getConfData()
{
    return this->conf;
}
