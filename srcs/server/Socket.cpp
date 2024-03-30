/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 12:19:12 by hoigag            #+#    #+#             */
/*   Updated: 2024/03/30 22:00:02 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"


unsigned int convertIpToInt(std::string ip)
{
   unsigned int result = 0;
    while (ip.size() > 0)
    {
        size_t pos = ip.find(".");
        if (pos == std::string::npos)
        {
            result = result * 256 + std::stoi(ip);
            break;
        }
        result = result * 256 + std::stoi(ip.substr(0, pos));
        ip = ip.substr(pos + 1);
    }
    return result;
}
short Socket::getPort()
{
    return this->port;
}

void Socket::setPort(short port)
{
    this->port = port;
}

Socket::Socket()
{
}

Socket::Socket(short port, std::string& hostname)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        std::runtime_error("could not create socket");
    setSocketToNonBlocking(sock);
    setSocketToBeReusable(sock);
    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    hostname = (hostname == "localhost" ? "127.0.0.1" : hostname);
    servaddr.sin_addr.s_addr = htonl(convertIpToInt(hostname));
    servaddr.sin_port = htons(port);
    if(bind(sock, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
    {
        std::string error =  "could not bind socket " + itoa(sock);
        throw std::runtime_error(error);
    }
    if ((listen(sock, SOMAXCONN)) < 0)
        throw std::runtime_error("could not listen on socket");
    this->fd = sock;
}

int Socket::acceptNewConnetction(int serverFd)
{
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    int connFd = accept(serverFd,  (struct sockaddr *)&addr, (socklen_t *) &addr_len);
    if (connFd < 0)
        std::runtime_error("could not accept the connection");
    setSocketToNonBlocking(connFd);
    return connFd;
}

int Socket::getFd()
{
    return this->fd;
}

Socket::~Socket()
{

}
