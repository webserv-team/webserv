/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/25 13:25:34 by hoigag            #+#    #+#             */
/*   Updated: 2024/03/20 14:02:53 by hoigag           ###   ########.fr       */
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

// /images/faces/men/face1.jpg
// Location isCorrectLocation(std::string& uri, std::vector<Location>& locations)
// {
//     for (size_t i = 0; i < locations.size(); i++)
//     {
//         std::string locationUri = locations[i].path;
//     }
//     return Location();
// }


Response	WebServer::formResponse(Request& req)
{
    // std::cout << "forming response " << std::endl;
    Response response;
    std::string content;
    std::string header;
    std::string contentType;
    std::string queryString = "";
    ConfigData conf = this->getServer(req.getPort()).getConfData();
    // std::cout << "got the right conf data " << std::endl;
    std::string resourceFullPath = conf.root;
    // std::cout << conf.locations[0].index << std::endl;
    std::string url = req.getURL();
    size_t pos = url.find("?");
    // std::cout << "url == " << url << std::endl;
    // if (req.getMethod() == "POST" && req.getContentType().find("multipart/form-data") != string::npos)
    //     uploadFiles(req);
    std::cout << "url == " << url << std::endl;
    if (url == "/") 
        url = "/index.html";
    if (pos != std::string::npos)
    {
        resourceFullPath += url.substr(0, pos);
        queryString = url.substr(pos + 1);
    }
    else
        resourceFullPath += url;

    // std::cout << "locations" << std::endl;
    // for (size_t i = 0; i < conf.locations.size(); i++)
    // {
    //     std::cout << conf.locations[i].path << std::endl;
    //     std::cout << conf.locations[i].root << std::endl;
    //     std::cout << conf.locations[i].index << std::endl;
    // }
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
    std::cout << response << std::endl;
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

void WebServer::handleExistingConnection(int fd)
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
        if (this->clients[fd].headerObject.getMethod() == "GET")
            this->clients[fd].isRequestFinished = true;
        else if (this->clients[fd].headerObject.getMethod() == "POST")
        {
            // std::cout << RED << "before content length == " << this->clients[fd].headerObject.getContentLength() << " |||||    bytesread == " << this->clients[fd].bytesRead << RESET << std::endl; 
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
    // std::cout << "header == " << this->clients[fd].headerObject.getContentLength() << std::endl;
    // std::cout << "content length == " << this->clients[fd].bytesRead << std::endl;
    // exit(125);
    // else
    //     this->clients[fd].header.append(dataRead);
    if (this->clients[fd].isRequestFinished)
    {
        // std::cout<< this->clients[fd].request;
        FD_CLR(fd, &read_sockets);
        FD_SET(fd, &write_sockets);
        // std::cout << RED << this->clients[fd].request << RESET <<std::endl;
        Request req(this->clients[fd].request);
        std::cout << req;
		//Response response(reaq, std::vector<Socket>& servers)
        this->clientResponses[fd].response = this->formResponse(req);
        this->clientResponses[fd].responseSize = this->clientResponses[fd].response.getResponseLength();
        this->clientResponses[fd].totalDataSent = 0;
        this->clientResponses[fd].isResponseFinished = false;
    }
        // std::cout << GREEN << "request finished" << RESET << std::endl;
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

