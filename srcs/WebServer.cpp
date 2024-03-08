/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hassan <hassan@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/25 13:25:34 by hoigag            #+#    #+#             */
/*   Updated: 2024/03/08 23:40:21 by hassan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include <iostream>
#include <fstream>
#include "helpers.hpp"
#include "Cgi.hpp"
#include "Response.hpp"
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
        content = loadFile(conf.errorPages["404"]);
        response.setStatusCode(404);
    }
    else if (access(resourceFullPath.c_str(), R_OK) != 0)
    {
        content = loadFile(conf.errorPages["403"]);
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
    Client newClient = {"" , false, 0, 0, "", "", false, Header()};
    this->clients[connFd] = newClient;
}

void WebServer::handleExistingConnection(int fd)
{
    std::string dataRead = sread(fd);
    this->clients[fd].request.append(dataRead);
    size_t carr_pos = dataRead.find("\r\n\r\n");
    std::cout << "<<data read == " << dataRead << ">>" <<std::endl;
    if (!this->clients[fd].isHeaderFinished && carr_pos != std::string::npos)
    {
        std::cout << "header finished" << std::endl;
        this->clients[fd].isHeaderFinished = true; 
        std::string header = this->clients[fd].request.substr(0, carr_pos);
        this->clients[fd].headerObject = Header(header);
        std::string content = this->clients[fd].request.substr(carr_pos + 4);
        this->clients[fd].bytesRead = content.length();
    }
    if (this->clients[fd].isHeaderFinished)
    {
        if (this->clients[fd].headerObject.getMethod() == "GET")
            this->clients[fd].isRequestFinished = true;
        else if (this->clients[fd].headerObject.getMethod() == "POST")
        {
                std::cout << "content length == " << this->clients[fd].headerObject.getContentLength() << std::endl;
                std::cout << " bytesread == " << this->clients[fd].bytesRead << std::endl;
            if (this->clients[fd].bytesRead >= this->clients[fd].headerObject.getContentLength())
            {
                this->clients[fd].isRequestFinished = true;
                
            }
            else
                this->clients[fd].bytesRead += dataRead.length();
        }
            
    }
    // std::cout << "header == " << this->clients[fd].headerObject.getContentLength() << std::endl;
    // std::cout << "content length == " << this->clients[fd].bytesRead << std::endl;
    // exit(125);
    // else
    //     this->clients[fd].header.append(dataRead);
    if (this->clients[fd].isRequestFinished)
    {
        std::cout << "request finished" << std::endl;
        
        std::cout<< this->clients[fd].request;
        FD_CLR(fd, &read_sockets);
        FD_SET(fd, &write_sockets);
        Request req(this->clients[fd].request);
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

