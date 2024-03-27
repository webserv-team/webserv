/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServersMonitor.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/25 13:25:34 by hoigag            #+#    #+#             */
/*   Updated: 2024/03/27 22:10:26 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServersMonitor.hpp"
#include <iostream>
#include <fstream>
#include "helpers.hpp"
#include "Cgi.hpp"
#include "Response.hpp"
ServersMonitor::ServersMonitor()
{
    
}

HttpServer ServersMonitor::getServer(int port)
{
    int index = 0;
    for (size_t i = 0; i < this->httpServers.size(); i++)
    {
        std::vector<int> ports = this->httpServers[i].getConfData().ports;
        for (size_t j = 0; j < ports.size(); j++)
        {
            if (ports[j] == port && this->httpServers[i].getConfData().host == "localhost")
                index = i;
        }
    }
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

void ServersMonitor::handleNewConnection(int serverFd)
{
    int connFd = Socket::acceptNewConnetction(serverFd);
    setSocketToNonBlocking(connFd);
    FD_SET(connFd, &this->read_sockets);
    if (connFd > this->maxFd)
        this->maxFd = connFd;
    Client newClient;
    newClient.request = "";
    newClient.bytesRead = 0;
    newClient.headerObject = Header();
    newClient.isBody = false;
    newClient.isHeaderFinished = false;
    newClient.isRequestFinished = false;
    newClient.contentlength = 0;
    this->clients[connFd] = newClient;
}

void ServersMonitor::handleExistingConnection(int fd)
{
    std::string dataRead = sread(fd);
    this->clients[fd].request.append(dataRead);
    size_t carr_pos = dataRead.find("\r\n\r\n");
    // std::cout << "<<data read == " << dataRead << ">>" <<std::endl;
    // std::cout << "<< size of data read == " << dataRead.length() << ">>" <<std::endl;

        if (!this->clients[fd].isHeaderFinished && carr_pos != std::string::npos)
        {
            // std::cout << GREEN << "header finished" << RESET <<  std::endl;
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
                // std::cout << RED << "before content length == " << this->clients[fd].headerObject.getContentLength() << " |||||    bytesread == " << this->clients[fd].bytesRead << RESET << std::endl; 
                if (this->clients[fd].headerObject.isChunked())
                {
                    if (dataRead.find("0\r\n\r\n") != std::string::npos)
                    {
                        // std::cout << "request chunked finished" << std::endl;
                        this->clients[fd].isRequestFinished = true;
                    }
                }
                else
                {
                    if (this->clients[fd].isBody)
                        this->clients[fd].bytesRead += dataRead.length();
                    else
                        this->clients[fd].isBody = true;
                    if (this->clients[fd].bytesRead >= this->clients[fd].headerObject.getContentLength())
                    {
                        // std::cout << GREEN <<"request finished" << RESET << std::endl;    
                        this->clients[fd].isRequestFinished = true;
                    }
                    // std::cout << "IS REQUEST FINISHED: " << this->clients[fd].isRequestFinished << std::endl;
                    std::cout << RED << "content length == " << this->clients[fd].headerObject.getContentLength() << " |||||    bytesread == " << this->clients[fd].bytesRead << RESET << std::endl;        
                }
            }
                
        }
        // std::cout << "header == " << this->clients[fd].headerObject.getContentLength() << std::endl;
        // std::cout << "content length == " << this->clients[fd].bytesRead << std::endl;
        // exit(125);
        // else
        //     this->clients[fd].header.append(dataRead);
        if (this->clients[fd].isRequestFinished)
        {
            // std::cout << "request " << this->clients[fd].headerObject.getMethod() << " finished" << std::endl;
            // std::cout<< this->clients[fd].request;
            FD_CLR(fd, &read_sockets);
            // this->clients.erase(fd);
            FD_SET(fd, &write_sockets);
            // std::cout << RED << this->clients[fd].request << RESET <<std::endl;
            Request req(this->clients[fd].request);
            std::cout << req;
            std::cout << this->clients[fd].request << std::endl;
            ConfigData conf = this->getServer(req.getPort()).getConfData();
            Response res(req, conf);
            std::cout << res;
            //Response response(reaq, std::vector<Socket>& servers)
            this->clientResponses[fd].response = res.getResponseString();
            this->clientResponses[fd].responseSize = this->clientResponses[fd].response.length();
            this->clientResponses[fd].totalDataSent = 0;
            this->clientResponses[fd].isResponseFinished = false;
        }
            // std::cout << GREEN << "request finished" << RESET << std::endl;
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
        struct timeval timeout;
        timeout.tv_sec = 20;
        timeout.tv_usec = 0;
        while (true)
        {
        this->read_copy_sockets = this->read_sockets;
        this->write_copy_sockets = this->write_sockets;
        int res = select(this->maxFd + 1, &this->read_copy_sockets, &this->write_copy_sockets, NULL, &timeout);
        if (res < 0)
        {
            std::cerr << "error occured on select" << std::endl;
            exit(1);
        }
        else if (res == 0)
            std::runtime_error("timeout occured");
        for (int i = 0; i <= this->maxFd; i++)
        {
            if (FD_ISSET(i, &this->read_copy_sockets))
            {
                if (this->isServerFd(i))
                    this->handleNewConnection(i);
                else
                {
                    try
                    {
                        this->handleExistingConnection(i);
                    }
                    catch(const std::exception& e)
                    {
                        std::cerr << e.what() << '\n';
                    }
                    
                    // this->handleExistingConnection(i);
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
                    // this->clientResponses.erase(i);
                    // this->clients.erase(i);
                    close(i);
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

}



// ServersMonitor::ServersMonitor(const ServersMonitor& other)
// {

// }

// ServersMonitor& ServersMonitor::operator=(const ServersMonitor& other)
// {

// }

