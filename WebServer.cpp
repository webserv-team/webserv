/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/25 13:25:34 by hoigag            #+#    #+#             */
/*   Updated: 2024/01/23 13:45:07 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include <iostream>
#include <fstream>
#include "unistd.h"
#include "fcntl.h"

std::vector<char> loadBinary(const std::string& path) {
    char buffer;
    std::vector<char> data;
    int fd  = open(path.c_str(), O_RDONLY);
    int n = 1;
    while (n)
    {
        n = read(fd, &buffer, 1);
        if (n < 0)
            break;
        data.push_back(buffer);
    }
    return data;
}

std::string loadFile(std::string file)
{
    std::cout << file << std::endl;
    std::ifstream inFile(file);
    if (!inFile.is_open())
        throw std::runtime_error("could not open file");
    std::string outputString = "";
    while (inFile)
    {
        std::string line;
        std::getline(inFile, line);
        outputString += line;
    }
    return outputString;
}

void	WebServer::sendResponse(Request req, int sock)
{
    std::string content = "";
    std::string contentType = "text/html";
    std::string statusLine = "HTTP/1.1 200\r\n";
    std::cout << this->server.documentRoot << std::endl;
    try
    {
        if (req.getURL() == "/")
            content = loadFile(this->server.documentRoot + "/index.html");
        else if (req.getURL().find(".jpg") != std::string::npos)
        {
            // std::vector<char> image_data = loadBinary(this->server.documentRoot + req.getURL()); 
            // std::string c(image_data.begin(), image_data.end());
            // std::cout << c << std::endl;
            std::vector<char> data = loadBinary(this->server.documentRoot + req.getURL());
            std::string sdata(data.begin(), data.end());
            std::cout << "data: " << sdata << std::endl;
            std::cout << "size: " << sdata.size() << std::endl;
            content += sdata;
        }
        else
            content = loadFile(this->server.documentRoot + req.getURL());
    }
    catch(const std::exception& e)
    {
        content = loadFile(this->server.errorPages["not_found"]);
        statusLine = "HTTP/1.1 404 KO\r\n";
    }
    
    if (req.getURL().find(".css") != std::string::npos)
        contentType = "text/css";
    else if (req.getURL().find(".png") != std::string::npos)
        contentType = "image/png";
    else if (req.getURL().find(".jpg") != std::string::npos)
        contentType = "image/jpeg";
    else if (req.getURL().find(".html") != std::string::npos)
        contentType = "text/html";
	std::string response = "" + statusLine;
    response += "Content-Type: ";
    response += contentType;
    response += "\r\n";
    response += "Content-Length: " + std::to_string(content.length()) + "\r\n";
    response += "\r\n";
    response += content;

	if (send(sock, response.c_str(), strlen(response.c_str()), 0) < 0)
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
        std::cout << "listening on port " << this->server.port << std::endl;
        connFd = accept(this->listenFD,  (struct sockaddr *)&addr, (socklen_t *) &addr_len);
        if (connFd < 0)
        {
            std::cout << "could not accept the connection" << std::endl;
            exit(1);
        }
        std::string request = getRequest(connFd);
        std::cout << request << std::endl;
        Request req(request);
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

