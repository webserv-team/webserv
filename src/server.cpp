/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emohamed <emohamed@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/23 11:26:17 by emohamed          #+#    #+#             */
/*   Updated: 2024/01/29 17:02:22 by emohamed         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <stdlib.h>
#include <netinet/in.h>
#include <string>
#include <string.h>
#include <sstream>
#include <fstream>
#include <vector>
#include <fcntl.h>
#include "../parsing/ServerConf.hpp"
#include "../parsing/Request.hpp"
struct sockaddr_in address;
#define BUFFER_SIZE 10000

#define GREEN "\033[1;32m"
#define RED "\033[1;31m"
#define YELLOW "\033[1;33m"
#define RESET "\033[0m"

std::string getContentType(const std::string& filePath)
{
	std::string extension = filePath.substr(filePath.find_last_of(".") + 1);
	if (extension == "jpg" || extension == "jpeg")
		return "image/jpeg";
	else if (extension == "png")
		return "image/png";
	else if (extension == "gif")
		return "image/gif";
	else if (extension == "html")
		return "text/html";
	else
		return "application/octet-stream";
}
void defaultError(int socket, int statusCode)
{
	std::string errorResponse = "HTTP/1.1 " + std::to_string(statusCode) + " Internal Server Error\r\n";
	errorResponse += "Content-Type: text/html\r\n\r\n";
	errorResponse += "<!DOCTYPE html>"
		"<html>"
		"<head>"
			"<title>Error</title>"
			"<style>"
				"*{"
					"transition: all 0.6s;"
				"}"
				"html {"
					"height: 100%;"
				"}"
				"body{"
					"font-family: 'Lato', sans-serif;"
					"color: #888;"
					"margin: 0;"
				"}"
				"#main{"
					"display: table;"
					"width: 100%;"
					"height: 100vh;"
					"text-align: center;"
				"}"
				".fof{"
					"display: table-cell;"
					"vertical-align: middle;"
				"}"
				".fof h1{"
					"font-size: 50px;"
					"display: inline-block;"
					"padding-right: 12px;"
					"animation: type .5s alternate infinite;"
				"}"
				"@keyframes type{"
					"from{box-shadow: inset -3px 0px 0px #888;}"
					"to{box-shadow: inset -3px 0px 0px transparent;}"
				"}"
			"</style>"
		"</head>"
		"<body>"
			"<div id=\"main\">"
				"<div class=\"fof\">"
					"<h1>Error " + std::to_string(statusCode) + "</h1>"
				"</div>"
			"</div>"
		"</body>"
		"</html>";
	if (send(socket, errorResponse.c_str(), errorResponse.length(), 0) < 0)
		std::cerr << RED << "Error sending error response: " << strerror(errno) << RESET << std::endl;
}
void error500(int socket, std::string& filePath)
{
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		std::cerr << "Error page not found" << std::endl;
		defaultError(socket, 500);
		return ;
	}
	std::stringstream fileContent;
	fileContent << file.rdbuf();
	std::string content = fileContent.str();
	file.close();
	std::string response = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\nContent-Length: ";
	response += std::to_string(content.length()) + "\r\n\r\n" + content;
	if (send(socket, response.c_str(), response.length(), 0) < 0)
	{
		std::cerr << "Send error: " << strerror(errno) << std::endl;
		return ;
	}
}
void sendResponse(int socket, Request& request, ConfigData& server)
{
	std::string filePath = server.documentRoot;
	if (request.getURL() == "/")
		filePath += "/home.html";
	else
		filePath += request.getURL();

    std::cout <<  "---->   " << filePath << std::endl;
	std::ifstream file(filePath);
	if (file.is_open())
	{
		std::stringstream fileContent;
		fileContent << file.rdbuf();
		std::string content = fileContent.str();
		file.close();
		
		std::string response = "HTTP/1.1 200 OK\r\nContent-Type: " + getContentType(filePath) + "\r\nContent-Length: ";
		response += std::to_string(content.length()) + "\r\n\r\n" + content;
		if (send(socket, response.c_str(), response.length(), 0) < 0)
		{
			std::cerr << RED << "Send error: " << strerror(errno) << RESET << std::endl;
			error500(socket, server.errorPages["server_error"]);
			return;
		}	
		std::cout << GREEN << "Response sent : " << RESET << RED << request.getURL() << RESET << std::endl;
	}
    else
	{
		std::string tmp = server.errorPages["not_found"];
		file.open(tmp);
		if (!file.is_open())
		{		std::cerr << RED << "Error page not found: " << strerror(errno) << RESET << std::endl;
			error500(socket, server.errorPages["server_error"]);
			return ;
		}
		std::stringstream fileContent;
		fileContent << file.rdbuf();
		std::string content = fileContent.str();
		file.close();
		std::string response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: ";
		response += std::to_string(content.length()) + "\r\n\r\n" + content;
		
		if (send(socket, response.c_str(), response.length(), 0) < 0)
		{
			std::cerr << RED << "Send error: " << strerror(errno) << RESET << std::endl;
			error500(socket, server.errorPages["server_error"]);
			return ;
		}
	}
}

int main(){
    ServerConf data("../default.toml");
    int port = data.getServers()[0].port;
    int new_socket;
    int server = socket(AF_INET, SOCK_STREAM, 0);
    if(server < 0){
        std::cerr << "Socket creation error" << std::endl;
        exit(1);
    }
    int opt = 1;
    if(setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1){
        std::cerr << "Setsockopt eroor: " << strerror(errno) << std::endl;
        return -1;
    }
    // memset((char *)&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);
    int b = bind(server, (struct sockaddr *)&address, sizeof(address));
     if(b < 0){
        std::cerr << "Bind failed" << std::endl;
        exit(1);
    }
    int l = listen(server, 3);
	if(l < 0){
		std::cerr << "Listen failed" << std::endl;
		exit(1);
	}
    socklen_t addrlen = sizeof(address);
    std::vector<ConfigData> servers = data.getServers();
	signal(SIGPIPE, SIG_IGN);
    char buff[BUFFER_SIZE];
    while(true){
        std::cout << "          \033[1;32m 🛠 ...... SERVER ON ...... 🛠\033[0m" << std::endl;
        new_socket = accept(server, (struct sockaddr *)&address, &addrlen);
		if (new_socket < 0) {
			std::cerr << "Accept error: " << strerror(errno) << std::endl;
			return (-1);
		}
		// int flags = fcntl(new_socket, F_GETFL, 0);
		// fcntl(new_socket, F_SETFL, flags | O_NONBLOCK);
		// fcntl(fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
		if(l < 0 || new_socket < 0){
            std::cerr << "cant accept or lestining .." << std::endl;
        }
        // int r = read(new_socket, buff, BUFFER_SIZE);
		int  r = recv(new_socket, buff, BUFFER_SIZE, 0);
        if( r < 0){
            std::cerr << "cant read from the socket" << std::endl;
        }
        buff[r] = '\0';
        // std::cout << buff << std::endl;
        std::cout << YELLOW << buff << RESET<< std::endl;
        Request req(buff);
        req.parseRequest();
        std::string path = req.getURL();
        for (size_t i = 0; i < servers.size(); i++)
		{
			if (servers[i].port == port)
			{
				sendResponse(new_socket, req, servers[i]);
			}
		}
		close(new_socket);
    }
	close(server);
}