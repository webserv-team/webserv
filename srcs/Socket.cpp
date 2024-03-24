/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 12:19:12 by hoigag            #+#    #+#             */
/*   Updated: 2024/03/21 20:01:09 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

Socket::Socket()
{
}
Socket::Socket(const ConfigData& conf)
{
    this->conf = conf;
}

int Socket::start()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        std::cerr << "could not create server socket" << std::endl;
        exit(1);
    }
    setSocketToNonBlocking(sock);
    setSocketToBeReusable(sock);
    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(this->conf.ports[0]);
    if(bind(sock, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
    {
        std::cerr << "could not bind socket " << sock << std::endl; 
        exit(1);
    }
    if ((listen(sock, SOMAXCONN)) < 0)
    {
        std::cerr << "error happened during listening for requests" << std::endl;
        exit(1);
    }
    this->fd = sock;
    std::cout << "listening on port : " << conf.ports[0] << std::endl;
    return sock;
}

int Socket::acceptNewConnetction(int serverFd)
{
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    int connFd = accept(serverFd,  (struct sockaddr *)&addr, (socklen_t *) &addr_len);
    if (connFd < 0)
    {
        std::cout << "could not accept the connection" << std::endl;
        exit(1);
    }
    setSocketToNonBlocking(connFd);
    return connFd;
}

int Socket::getFd()
{
    return this->fd;
}

ConfigData Socket::getConfData()
{
    return this->conf;
}

Socket::~Socket()
{
    // close();
}

// Socket::Socket(const Socket& other)
// {

// }

// Socket& Socket::operator=(const Socket& other)
// {

// }

