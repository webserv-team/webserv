/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/25 13:25:34 by hoigag            #+#    #+#             */
/*   Updated: 2024/03/07 18:49:28 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include <iostream>
#include <fstream>
#include "helpers.hpp"
#include "Cgi.hpp"
#include "Response.hpp"
#include "Header.hpp"
// WebServer::WebServer()
// {
    
// }

Response	WebServer::formResponse(Request& req)
{
    std::cout << "forming response " << std::endl;
    Response response;
    std::string content;
    std::string header;
    std::string contentType;
    ConfigData conf = this->getServer(req.getPort()).getConfData();
    std::cout << "got the right conf data " << std::endl;
    std::string resourceFullPath = conf.root;
    std::string url = req.getURL();
    size_t pos = url.find("?");
    // if (req.getMethod() == "POST" && req.getContentType().find("multipart/form-data") != string::npos)
    //     uploadFiles(req);
    if (url == "/")
        url = "/index.html";
    if (pos != std::string::npos)
        resourceFullPath += url.substr(0, pos);
    else
        resourceFullPath += url;
    if (access(resourceFullPath.c_str(), F_OK) != 0)    
    {
        content = loadFile(conf.errorPages["not_found"]);
        response.setStatusCode(404);
    }
    else if (access(resourceFullPath.c_str(), R_OK) != 0)
    {
        content = loadFile(conf.errorPages["forbidden"]);
        response.setStatusCode(403);
    }
    else if (isDirectory(resourceFullPath))
        content = directoryListing(resourceFullPath);
    else if (isSupportedCgiScript(resourceFullPath))
    {
        std::cout << "this is a cgi script" << std::endl;
        Cgi cgi(req);
        content = cgi.executeScript(resourceFullPath);
        size_t pos = content.find("\r\n\r\n");
        if (pos != std::string::npos)
        {
            content = content.substr(pos);
            header = content.substr(0, pos);
        }
    }
    else
        content = loadFile(conf.root + url);
    if (!isSupportedCgiScript(url))
    {
        std::string ext = getFileExtension(url);
        contentType = mimes.getContentType(ext);
    }
    else if (isSupportedCgiScript(url))
        contentType = getContentTypeFromCgiOutput(header);
    response.setContentType(contentType);
    response.setContentLength(content.size());
    response.setBody(content);
    response.buildResponse();
    return response;
}

Socket WebServer::getServer(int port)
{
    int index = -1;
    for (size_t i = 0; i < this->httpServers.size(); i++)
    {
        if (this->httpServers[i].getConfData().ports[0] == port)
            index = i;
    }
    return this->httpServers[index];
}

WebServer::WebServer(std::vector<Socket>& httpServers) 
{
    FD_ZERO(&this->read_sockets);
    FD_ZERO(&this->write_sockets);
    this->httpServers = httpServers;
    this->maxFd = 0;
    for (size_t i = 0; i < this->httpServers.size(); i++)
    {
        this->httpServers[i].start();
        int serverFD = this->httpServers[i].getFd();
        FD_SET(serverFD, &read_sockets);
        if (serverFD > this->maxFd)
            this->maxFd = serverFD;  
    }
}

bool WebServer::isServerFd(int fd)
{
    for (size_t i = 0; i < this->httpServers.size(); i++)
    {
        if (this->httpServers[i].getFd() == fd)
            return true;
    }
    return false;
}

void WebServer::handleNewConnection(int serverFd)
{
    int connFd = Socket::acceptNewConnetction(serverFd);
    setSocketToNonBlocking(connFd);
    FD_SET(connFd, &this->read_sockets);
    if (connFd > this->maxFd)
        this->maxFd = connFd;
    Client newClient = {"" , false, 0, 0, "", "", false};
    this->clients[connFd] = newClient;
}

void WebServer::handleExistingConnection(int fd)
{
    std::string dataRead = sread(fd);
    Header headers;
    size_t carr_pos = dataRead.find("\r\n\r\n");
    if (carr_pos != std::string::npos)
    {
        this->clients[fd].isHeaderFinished = true; 
        this->clients[fd].header += dataRead.substr(0, carr_pos);
        this->clients[fd].content = dataRead.substr(carr_pos + 4);
        this->clients[fd].bytesRead = this->clients[fd].content.length();
    }
    if (this->clients[fd].isHeaderFinished)
    {
        headers =  Header(this->clients[fd].header);   
        if (headers.getMethod() == "GET")
            this->clients[fd].isRequestFinished = true;
        else if (headers.getMethod() == "POST")
        {
            if (dataRead.find("\r\n\r\n") == std::string::npos)
            {
                this->clients[fd].content.append(dataRead);
                this->clients[fd].bytesRead += dataRead.length();
            }
            if (this->clients[fd].bytesRead >= headers.getContentLength())
            {
                std::cout << "POST REQUEST END" << std::endl;
                this->clients[fd].isRequestFinished = true;
            }
        }
    }
    else
        this->clients[fd].header.append(dataRead);
    if (this->clients[fd].isRequestFinished)
    {
        FD_CLR(fd, &read_sockets);
        FD_SET(fd, &write_sockets);
        std::string httprequest = this->clients[fd].header + "\r\n\r\n" + this->clients[fd].content;
        Request req(httprequest);
        std::cout << httprequest << std::endl;
        std::cout << "length === " << req.getContentLength() << std::endl;
        // std::cout << req;
        this->clientResponses[fd].response = this->formResponse(req);
        this->clientResponses[fd].responseSize = this->clientResponses[fd].response.getResponseLength();
        this->clientResponses[fd].totalDataSent = 0;
        this->clientResponses[fd].isResponseFinished = false;
    }
}

int sendChunk(int sock, ClientResponse& cr)
{
    int dataSent = 0;
    dataSent = send(sock, cr.response.getResponseString().c_str() + cr.totalDataSent, cr.responseSize - cr.totalDataSent, 0);
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

void WebServer::listenForConnections()  
{
    while (true)
    {
        this->read_copy_sockets = this->read_sockets;
        this->write_copy_sockets = this->write_sockets;
        if (select(this->maxFd + 1, &this->read_copy_sockets, &this->write_copy_sockets, NULL, NULL) < 0)
        {
            std::cerr << "error occured on select" << std::endl;
            exit(1);
        }
        for (int i = 0; i <= this->maxFd; i++)
        {
            if (FD_ISSET(i, &this->read_copy_sockets))
            {
                if (this->isServerFd(i))
                    this->handleNewConnection(i);
                else
                    this->handleExistingConnection(i);
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
                }
            }
        }
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

