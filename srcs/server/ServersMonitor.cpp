/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServersMonitor.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/25 13:25:34 by hoigag            #+#    #+#             */
/*   Updated: 2024/03/31 01:00:34 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServersMonitor.hpp"

ServersMonitor::ServersMonitor()
{
    
}



HttpServer ServersMonitor::getServer(Request& req, short port)
{
    int index = -1;
    for (size_t i = 0; i < this->httpServers.size(); i++)
    {
        std::string reqHost = req.getHostName() == "localhost" ? "127.0.0.1" : req.getHostName();
        if (this->httpServers[i].getConfData().host == reqHost || this->httpServers[i].getConfData().serverName == reqHost)
        {
            std::vector<int> ports = this->httpServers[i].getConfData().ports;
            for (size_t j = 0; j < ports.size(); j++)
            {
                if (ports[j] == port)
                {
                    index = i;
                    break;
                }
            }
        }
    }
    if (index == -1)
        throw std::runtime_error("server not found");
    return this->httpServers[index];
}

ServersMonitor::ServersMonitor(std::vector<HttpServer>& httpServers) 
{
    FD_ZERO(&this->read_sockets);
    FD_ZERO(&this->write_sockets);
    this->httpServers = httpServers;
    this->maxFd = 0;
    for (size_t i = 0; i < this->httpServers.size(); i++)
        this->httpServers[i].start(this->read_sockets, this->maxFd);
}

bool ServersMonitor::isServerFd(int fd)
{
    for (size_t i = 0; i < this->httpServers.size(); i++)
    {
        std::vector<Socket> sockets = this->httpServers[i].getSockets();
        for (size_t j = 0; j < sockets.size(); j++)
        {
            if (sockets[j].getFd() == fd)
                return true;
        }
    }
    return false;
}
void initClientStruct(Client& client)
{
    client.request = "";
    client.bytesRead = 0;
    client.headerObject = Header();
    client.isBody = false;
    client.isHeaderFinished = false;
    client.isRequestFinished = false;
    client.contentlength = 0;
}
void ServersMonitor::handleNewConnection(int serverFd)
{
    int connFd = Socket::acceptNewConnetction(serverFd);
    setSocketToNonBlocking(connFd);
    FD_SET(connFd, &this->read_sockets);
    if (connFd > this->maxFd)
        this->maxFd = connFd;
    Client newClient;
    for (size_t i = 0; i < this->httpServers.size(); i++)
    {
        std::vector<Socket> sockets = this->httpServers[i].getSockets();
        for (size_t j = 0; j < sockets.size(); j++)
        {
            if (sockets[j].getFd() == serverFd)
            {
                newClient.port = sockets[j].getPort();
                break;
            }
        }
    }
    initClientStruct(newClient);
    this->clients[connFd] = newClient;
}

void ServersMonitor::handleExistingConnection(int fd)
{
    std::string dataRead = sread(fd);
    this->clients[fd].request.append(dataRead);
    size_t carr_pos = dataRead.find("\r\n\r\n");

    if (!this->clients[fd].isHeaderFinished && carr_pos != std::string::npos)
    {
        this->clients[fd].isHeaderFinished = true; 
        std::string header = this->clients[fd].request.substr(0, carr_pos);
        this->clients[fd].headerObject = Header(header);
        std::string content = this->clients[fd].request.substr(carr_pos + 4);
        this->clients[fd].bytesRead = content.length();
    }   
    if (this->clients[fd].isHeaderFinished)
    {
        if (this->clients[fd].headerObject.getMethod() == "GET" || this->clients[fd].headerObject.getMethod() == "DELETE")
            this->clients[fd].isRequestFinished = true;
        
        else if (this->clients[fd].headerObject.getMethod() == "POST")
        {
            if (this->clients[fd].headerObject.isChunked())
            {
                if (dataRead.find("0\r\n\r\n") != std::string::npos)
                    this->clients[fd].isRequestFinished = true;
            }
            else
            {
                if (this->clients[fd].isBody)
                    this->clients[fd].bytesRead += dataRead.length();
                else
                    this->clients[fd].isBody = true;
                if (this->clients[fd].bytesRead >= this->clients[fd].headerObject.getContentLength())
                    this->clients[fd].isRequestFinished = true;
                // std::cout << RED << "content length == " << this->clients[fd].headerObject.getContentLength() << " |||||    bytesread == " << this->clients[fd].bytesRead << RESET << std::endl;        
            }
        }
        else
            this->clients[fd].isRequestFinished = true;
            
    }
    if (this->clients[fd].isRequestFinished)
    {
        FD_CLR(fd, &read_sockets);
        FD_SET(fd, &write_sockets);
        Request req(this->clients[fd].request);
        std::cout << req;
        try
        {
            ConfigData conf = this->getServer(req, this->clients[fd].port).getConfData();
            initClientStruct(this->clients[fd]);
            Response res(req, conf);
            std::cout << res;
            this->clientResponses[fd].response = res.getResponseString();
            this->clientResponses[fd].responseSize = this->clientResponses[fd].response.length();
            this->clientResponses[fd].totalDataSent = 0;
            this->clientResponses[fd].isResponseFinished = false;
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        
    }
}

int sendChunk(int sock, ClientResponse& cr)
{
    int dataSent = 0;
    dataSent = send(sock, cr.response.c_str() + cr.totalDataSent, cr.responseSize - cr.totalDataSent, 0);
    if (dataSent < 0)
    {   
        std::cerr << "error: Could not send data" << std::endl;
        return 0;
    }
    cr.totalDataSent += dataSent;
    if (cr.totalDataSent >= cr.responseSize)
        cr.isResponseFinished = true;
    return 1;
}

void ServersMonitor::listenForConnections()  
{
    try
    {
        while (true)
        {
            this->read_copy_sockets = this->read_sockets;
            this->write_copy_sockets = this->write_sockets;
            int res = select(this->maxFd + 1, &this->read_copy_sockets, &this->write_copy_sockets, NULL, NULL);
            if (res < 0)
                throw std::runtime_error("select error");
            for (int i = 0; i <= this->maxFd; i++)
            {
                if (FD_ISSET(i, &this->read_copy_sockets))
                {
                    if (this->isServerFd(i))
                    {
                        try
                        {
                            this->handleNewConnection(i);
                        }
                        catch(const std::exception& e)
                        {
                            std::cerr << e.what() << '\n';
                        }            
                    }
                    else
                    {
                        try
                        {
                            this->handleExistingConnection(i);
                        }
                        catch(const std::exception& e)
                        {
                            std::cerr << e.what() << '\n';
                            close(i);
                            this->clients.erase(i);
                        }
                    }
                }
                if (FD_ISSET(i, &this->write_copy_sockets))
                {
                    int success = sendChunk(i, this->clientResponses[i]);
                    if (!success)
                    {
                        FD_CLR(i, &this->write_sockets);
                        close(i);
                    }
                    if (this->clientResponses[i].isResponseFinished)
                    {
                        FD_CLR(i, &this->write_sockets);
                        close(i);
                        clientResponses.erase(i);
                    }
                }
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

ServersMonitor::~ServersMonitor()
{
    for (size_t i = 0; i < this->httpServers.size(); i++)
    {
        std::vector<Socket> sockets = this->httpServers[i].getSockets();
        for (size_t j = 0; j < sockets.size(); j++)
            close(sockets[j].getFd());
    }
}



// ServersMonitor::ServersMonitor(const ServersMonitor& other)
// {

// }

// ServersMonitor& ServersMonitor::operator=(const ServersMonitor& other)
// {

// }

