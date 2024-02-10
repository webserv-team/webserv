/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emohamed <emohamed@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/23 11:26:17 by emohamed          #+#    #+#             */
/*   Updated: 2024/02/09 19:59:03 by emohamed         ###   ########.fr       */
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
#define BUFFER_SIZE 1024

#define GREEN "\033[1;32m"
#define RED "\033[1;31m"
#define YELLOW "\033[1;33m"
#define RESET "\033[0m"

struct client{
	int socket;
	std::string request;
	std::string response;
};

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
	else if(extension == "mp4"){
		return "video/mp4";
	}
	else
		return "text/plain";
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

    // std::cout <<  "---->   " << filePath << std::endl;
	std::ifstream file(filePath);
	if (file.is_open())
	{
		std::stringstream fileContent;
		fileContent << file.rdbuf();
		std::string content = fileContent.str();
		file.close();
		
		std::string response = "HTTP/1.1 200 OK\r\nContent-Type: " + getContentType(filePath) + "\r\nContent-Length: ";
		response += std::to_string(content.length()) + "\r\n\r\n" + content;
		// int sizes = 0;
		// if ((sizes = send(socket, response.c_str(), response.length(), 0)) < 0)
		// {
		// 	std::cerr << RED << "Send error: " << strerror(errno) << RESET << std::endl;
		// 	error500(socket, server.errorPages["server_error"]);
		// 	return;
		// }
		int sent = 0;
		int totalSent = 0;
		int responseLength = response.length();
		while (totalSent < responseLength) {
			sent = send(socket, response.c_str() + totalSent, responseLength - totalSent, 0);
			if (sent == -1) {
				if (errno == EPIPE) {
					std::cerr << RED << "Client disconnected unexpectedly: " << strerror(errno) << RESET << std::endl;
					close(socket);
					return;
				} else {
					std::cerr << RED << "Send error: " << strerror(errno) << RESET << std::endl;
					error500(socket, server.errorPages["server_error"]);
					return;
				}
    }
    totalSent += sent;
}
		std::cout << RED << "size sended : " << sent << RESET <<  " | " <<  YELLOW << "full size: "<<response.length()  << RESET<< std::endl;
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


int main(int ac, char **av){
	if(ac != 2){
		std::cerr << "Usage: ./server <config_file>" << std::endl;
		return 1;
	}
	ServerConf data(av[1]);
	int port = data.getServers()[0].port;
	int new_socket;
	int server = socket(AF_INET, SOCK_STREAM, 0);
	if (server < 0)
	{
		std::cerr  << RED << "Socket creation error: " << strerror(errno) << RESET << std::endl;
		return 1;
	}
	int f = fcntl(server, F_GETFL);
	if(f < 0){
		std::cerr << RED << "Fcntl error: " << strerror(errno) << RESET << std::endl;
	}
	if(fcntl(server, F_SETFL, f | O_NONBLOCK) < 0){
		std::cerr << RED << "Fcntl error: " << strerror(errno) << RESET << std::endl;
	}
	 int opt = 1;
    if(setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1){
        std::cerr << "Setsockopt eroor: " << strerror(errno) << std::endl;
        return -1;
    }
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	if (bind(server, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		std::cerr << RED << "Bind error: " << strerror(errno) << RESET << std::endl;
		return 1;
	}
	if (listen(server, 3) < 0)
	{
		std::cerr << RED << "Listen error: " << strerror(errno) << RESET << std::endl;
		return 1;
	}
	if (ac != 2) {
        std::cerr << "Usage: " << av[0] << " <config_file>\n";
        return 1;
    }

    ServerConf data(av[1]);
    std::vector<ConfigData> servers = data.getServers();
	std::vector<int> ports;
	for(size_t i = 0; i < servers.size(); i++){
		ports.push_back(servers[i].port);
	}
    for (std::vector<ConfigData>::iterator it = servers.begin(); it != servers.end(); ++it) {
        int port = ports[it - servers.begin()];
		std::cout << RED << "port: " << port << std::endl;
        sockaddr_in address;
        int server = socket(AF_INET, SOCK_STREAM, 0);
        if (server < 0) {
            std::cerr << RED << "Socket creation error: " << strerror(errno) << RESET << std::endl;
            continue;
        }

        int opt = 1;
        if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
            std::cerr << RED << "Setsockopt error: " << strerror(errno) << RESET << std::endl;
            continue;
        }
        memset(&address, 0, sizeof(address));
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);
        if (bind(server, (struct sockaddr *)&address, sizeof(address)) < 0) {
            std::cerr << RED << "Bind error: " << strerror(errno) << RESET << std::endl;
            continue;
        }

        if (listen(server, 3) < 0) {
            std::cerr << RED << "Listen error: " << strerror(errno) << RESET << std::endl;
            continue;
        }
	socklen_t addrlen = sizeof(address);
	std::vector<client> clients;
	std::vector<ConfigData> servers = data.getServers();
	signal(SIGPIPE, SIG_IGN);
	char buffer[BUFFER_SIZE] ;
	fd_set master;
	FD_ZERO(&master);
	FD_SET(server, &master);
	while(1){
		std::cout << GREEN << "Waiting for connections..." << RESET << std::endl;
		fd_set copy = master;
		
		if(select(FD_SETSIZE, &copy, nullptr, nullptr, nullptr) < 0){
			std::cerr << RED << "Select error: " << strerror(errno) << RESET << std::endl;
			return 1;
		}
		for(int i = 0; i < FD_SETSIZE; i++){
			if(FD_ISSET(i , &copy)){
				int sock = i;
				if(sock == server){
					int new_socket = accept(server, (struct sockaddr *)&address, &addrlen);
					if (new_socket < 0)
					{
						std::cerr << RED << "Accept error: " << strerror(errno) << RESET << std::endl;
						return 1;
					}
					int flags = fcntl(new_socket, F_GETFL, 0);
					if(flags < 0){
						std::cerr << RED << "Fcntl error: " << strerror(errno) << RESET << std::endl;
						return 1;
					}
					if(fcntl(flags, F_SETFL, O_NONBLOCK, FD_CLOEXEC) < 0){
						std::cerr << RED << "Fcntl error: " << strerror(errno) << RESET << std::endl;
						return 1;
					}
					FD_SET(new_socket, &master);
					client newClient;
					newClient.socket = new_socket;
					clients.push_back(newClient);
					
				}
				else{
					client newClient;
					for(std::vector<client>::iterator it = clients.begin(); it != clients.end(); it++){
						if(it->socket == sock){
							newClient = *it;
							break;
						}
					
					}
					int r = recv(newClient.socket, buffer, BUFFER_SIZE - 1, 0);
					if(r < 0){
						std::cerr << RED << "Recv error: " << strerror(errno) << RESET << std::endl;
						
					}
					buffer[r] = '\0';
					newClient.request += buffer;

					std::size_t found = newClient.request.find("\r\n\r\n");
					if (found != std::string::npos) {
						std::cout  << "\n"<<  GREEN <<" ***** Complete request received ***** " << "\n"<<  std::endl;
					}
					std::cout << YELLOW << newClient.request << RESET<<std::endl;
					Request request(newClient.request);
					request.parseRequest();
					for(size_t i = 0; i < servers.size(); i++){
						if(servers[i].port == port){
							sendResponse(sock, request, servers[i]);
						}
					}
					close(sock);
					FD_CLR(sock, &master);
				}
			}
		}
	}
	
	
    }
}





int main(int ac, char **av){
	if(ac != 2){
		std::cerr << "Usage: ./server <config_file>" << std::endl;
		return 1;
	}
	ServerConf data(av[1]);
	int port = data.getServers()[0].port;
	int new_socket;
	int server = socket(AF_INET, SOCK_STREAM, 0);
	if (server < 0)
	{
		std::cerr  << RED << "Socket creation error: " << strerror(errno) << RESET << std::endl;
		return 1;
	}
	// int f = fcntl(server, F_GETFL);
	// if(f < 0){
	// 	std::cerr << RED << "Fcntl error: " << strerror(errno) << RESET << std::endl;
	// }
	// if(fcntl(server, F_SETFL, f | O_NONBLOCK) < 0){
	// 	std::cerr << RED << "Fcntl error: " << strerror(errno) << RESET << std::endl;
	// }
	 int opt = 1;
    if(setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1){
        std::cerr << "Setsockopt eroor: " << strerror(errno) << std::endl;
        return -1;
    }
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	if (bind(server, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		std::cerr << RED << "Bind error: " << strerror(errno) << RESET << std::endl;
		return 1;
	}
	if (listen(server, 3) < 0)
	{
		std::cerr << RED << "Listen error: " << strerror(errno) << RESET << std::endl;
		return 1;
	}
	socklen_t addrlen = sizeof(address);
	std::vector<client> clients;
	std::vector<ConfigData> servers = data.getServers();
	signal(SIGPIPE, SIG_IGN);
	char buffer[BUFFER_SIZE] ;
	fd_set master;
	FD_ZERO(&master);
	FD_SET(server, &master);
	while(1){
		std::cout << GREEN << "Waiting for connections..." << RESET << std::endl;
		fd_set copy = master;
		
		if(select(FD_SETSIZE, &copy, nullptr, nullptr, nullptr) < 0){
			std::cerr << RED << "Select error: " << strerror(errno) << RESET << std::endl;
			return 1;
		}
		for(int i = 0; i < FD_SETSIZE; i++){
			if(FD_ISSET(i , &copy)){
				int sock = i;
				if(sock == server){
					new_socket = accept(server, (struct sockaddr *)&address, &addrlen);
					if (new_socket < 0)
					{
						std::cerr << RED << "Accept error: " << strerror(errno) << RESET << std::endl;
						return 1;
					}
					int flags = fcntl(new_socket, F_GETFL, 0);
					if(flags < 0){
						std::cerr << RED << "Fcntl error: " << strerror(errno) << RESET << std::endl;
						return 1;
					}
					if(fcntl(flags, F_SETFL, O_NONBLOCK, FD_CLOEXEC) < 0){
						std::cerr << RED << "Fcntl error: " << strerror(errno) << RESET << std::endl;
						return 1;
					}
					FD_SET(new_socket, &master);
					client newClient;
					newClient.socket = new_socket;
					clients.push_back(newClient);
					
				}
				else{
					client newClient;
					for(std::vector<client>::iterator it = clients.begin(); it != clients.end(); it++){
						if(it->socket == sock){
							newClient = *it;
							break;
						}
					
					}
					int r = recv(newClient.socket, buffer, BUFFER_SIZE - 1, 0);
					if(r < 0){
						std::cerr << RED << "Recv error: " << strerror(errno) << RESET << std::endl;
						
					}
					buffer[r] = '\0';
					newClient.request += buffer;

					std::size_t found = newClient.request.find("\r\n\r\n");
					if (found != std::string::npos) {
						std::cout  << "\n"<<  GREEN <<" ***** Complete request received ***** " << "\n"<<  std::endl;
					}
					std::cout << YELLOW << newClient.request << RESET<<std::endl;
					Request request(newClient.request);
					request.parseRequest();
					for(size_t i = 0; i < servers.size(); i++){
						if(servers[i].port == port){
							sendResponse(sock, request, servers[i]);
						}
					}
					close(sock);
					FD_CLR(sock, &master);
				}
			}
		}
	}
	
	
}