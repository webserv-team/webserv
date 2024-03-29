/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 12:19:12 by hoigag            #+#    #+#             */
/*   Updated: 2024/03/29 14:15:18 by hoigag           ###   ########.fr       */
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

Socket::Socket()
{
}
/// @brief 
/// @param port 
/// @param hostname 
Socket::Socket(short port, std::string& hostname)
{
    // std::cout << "address == " << convertIpToInt(hostname) << std::endl; 
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
    hostname = (hostname == "localhost" ? "127.0.0.1" : hostname);
    servaddr.sin_addr.s_addr = htonl(convertIpToInt(hostname));
    servaddr.sin_port = htons(port);
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
    // close();
}

// Socket::Socket(const Socket& other)
// {

// }

// Socket& Socket::operator=(const Socket& other)
// {

// }

