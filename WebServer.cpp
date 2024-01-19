/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/25 13:25:34 by hoigag            #+#    #+#             */
/*   Updated: 2024/01/19 18:50:19 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include <iostream>
#include <fstream>

std::string loadBinary(std::string path)
{
    std::ifstream inFile(path, std::ios::binary);
    if (!inFile.is_open())
        throw std::runtime_error("could not open image file");
    std::string image_data(
        (std::istreambuf_iterator<char>(inFile)),
        (std::istreambuf_iterator<char>())
    );
    inFile.close();
    std::cout << image_data << std::endl;
    return image_data;
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
        line += "\n";
        outputString += line;
    }
    return outputString;
}

void	WebServer::sendResponse(Request req, int sock)
{
    std::string content;
    std::string contentType = "text/html";
    std::string statusLine = "HTTP/1.1 200\r\n";
    std::cout << this->server.documentRoot << std::endl;
    try
    {
        if (req.getURL() == "/")
            content = loadFile(this->server.documentRoot + "/index.html");
        else if (req.getURL().find(".jpg") != std::string::npos)
            content = loadBinary(this->server.documentRoot + req.getURL());
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
    if (req.getURL().find(".jpg") != std::string::npos)
        contentType = "image/jpg";
	std::string response = "" + statusLine;
    response += "Content-Type: ";
    response += contentType;
        req.getURL().find(".html");
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

