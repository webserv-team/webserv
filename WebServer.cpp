/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/25 13:25:34 by hoigag            #+#    #+#             */
/*   Updated: 2024/01/30 17:16:51 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include <iostream>
#include <fstream>
#include "helpers.hpp"
#include "Cgi.hpp"
using namespace std;

void	WebServer::sendResponse(Request req, int sock)
{
    std::string content;
    std::string header;
    std::string contentType = "text/html";
    std::string statusLine = "HTTP/1.1 200\r\n";
    std::string resourceFullPath = this->server.documentRoot;
    std::string url = req.getURL();
    std::cout << "url ===== " << url << std::endl;
    try
    {
        size_t pos = url.find("?");
        if (url == "/")
            url = "/index.html";
        if (pos != std::string::npos)
            resourceFullPath += url.substr(0, pos);
        else
            resourceFullPath += url;
        if (isDirectory(resourceFullPath))
            content = directoryListing(resourceFullPath);
        else if (isSupportedCgiScript(resourceFullPath))
        {
            Cgi cgi(req);
            content = cgi.executeScript(resourceFullPath);
            size_t pos = content.find("\r\n\r\n");
            header = content.substr(0, pos);
            if (pos != std::string::npos)
                content = content.substr(pos);
        }
        else
            content = loadFile(this->server.documentRoot + url);
    }
    catch(const std::exception& e)
    {
        content = loadFile(this->server.errorPages["not_found"]);
        statusLine = "HTTP/1.1 404 KO\r\n";
    }
    if (!isSupportedCgiScript(req.getURL()))
        contentType = getContentType(getFileExtension(req.getURL()));
    else if (isSupportedCgiScript(req.getURL()))
        contentType = getContentTypeFromCgiOutput(header);
	std::string response = "" + statusLine;
    response += "Content-Type: ";
    response += contentType;
    response += "\r\n";
    response += "Content-Length: " + std::to_string(content.size()) + "\r\n";
    response += "\r\n";
    response += content;
    
	if (send(sock, response.c_str(), response.length(), 0) < 0)
	{
		std::cerr << "error: Could not send data" << std::endl;
		exit(1);
	}
    close(sock);
}

WebServer::WebServer(ConfigData ServerConf)
{
    this->server = ServerConf;
}

void WebServer::createSocket()
{
    this->listenFD = socket(AF_INET, SOCK_STREAM, 0);
    if (this->listenFD < 0)
        throw std::runtime_error("could not create server socket");
}

void WebServer::bindSocket()
{
    int opt = 1;
    if (setsockopt(this->listenFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        std::cerr << "Setsockopt error: " << strerror(errno) << std::endl;
        exit (5);
    }
    bzero(&this->servaddr, sizeof(this->servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(0x7f000001);
    servaddr.sin_port = htons(this->server.port);

    //*binding the socket to the server
    if(bind(this->listenFD, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
        throw std::runtime_error("could not bind to socket");
}
 std::string getRequest(int connFd)
 {
    char data[REQUEST_LENGTH];
    int n = read(connFd, data, REQUEST_LENGTH);
    if (n  < 0)
    {
        std::cout << "could not read the request" << std::endl;
        exit (1);
    }
    data[n] = '\0';
    std::string request(data);
    return request;
 }

void WebServer::listenForConnections()
{
    int connFd;
    // std::string html = loadFile("index.html");
    if ((listen(this->listenFD, 3)) < 0)
        std::runtime_error("error happened during listening for requests");
    while (true)
    {
        struct sockaddr_in addr;
        socklen_t addr_len;
        // std::cout << "listening on port " << this->server.port << std::endl;
        connFd = accept(this->listenFD,  (struct sockaddr *)&addr, (socklen_t *) &addr_len);
        if (connFd < 0)
        {
            std::cout << "could not accept the connection" << std::endl;
            exit(1);
        }
        std::string request = getRequest(connFd);
        if (request.empty())
            continue;
        Request req(request);
        std::cout << req;
        sendResponse(req, connFd);
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

