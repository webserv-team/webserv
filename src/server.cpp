/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emohamed <emohamed@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/23 11:26:17 by emohamed          #+#    #+#             */
/*   Updated: 2024/02/01 13:19:14 by emohamed         ###   ########.fr       */
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
    memset((char *)&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);
    int b = bind(server, (struct sockaddr *)&address, sizeof(address));
     if(b < 0){
        std::cerr << "Bind failed" << std::endl;
        exit(1);
    }
    int l = listen(server, 10);
	if(l < 0){
		std::cerr << "Listen failed" << std::endl;
		exit(1);
	}
    socklen_t addrlen = sizeof(address);
    std::vector<ConfigData> servers = data.getServers();
	signal(SIGPIPE, SIG_IGN);
    char buff[BUFFER_SIZE];
	fd_set master; // master file descriptor set
	FD_ZERO(&master); // clear the master set
	FD_SET(server, &master); // add the listener to the master set
// 	while(true){

//     // Make a copy of the master file descriptor set, because select() modifies the set
//     fd_set copy = master;
//     // Call select() to wait for activity on any of the sockets in the set
//     // The first parameter is ignored in modern implementations, but traditionally it's the highest-numbered file descriptor in any of the sets, plus one
//     // The second parameter is the set of file descriptors to monitor for read readiness
//     // The last three parameters are sets of file descriptors to monitor for write readiness, error conditions, and timeout, all of which we're not using
//     // int socketCount = select(server+1, &copy, nullptr, nullptr, nullptr);

//     // // Check if select() returned an error
//     // if(socketCount == -1){
//     //     std::cerr << "select error: " << strerror(errno) << std::endl;
//     //     return (-1);
//     // }

//     // Loop over all the sockets in the set
//     for(int i = 0; i < FD_SETSIZE; i++){
//         // Check if the socket is in the set
//         if(FD_ISSET(i, &copy)){
//             int sock = i;

//             // If the socket is the server socket, it means there's a new connection
//             if(sock == server){
//                 // Accept the new connection
//                 new_socket = accept(server, (struct sockaddr *)&address, &addrlen);

//                 // Check if accept() returned an error
//                 if (new_socket < 0) {
//                     std::cerr << "Accept error: " << strerror(errno) << std::endl;
//                     return (-1);
//                 }

//                 // Add the new socket to the master set
//                 FD_SET(new_socket, &master);
//             }
// 			else{
// 				// Receive data from the socket
// 				int  r = recv(sock, buff, BUFFER_SIZE, 0);
// 				// Check if recv() returned an error
// 				if( r < 0){
// 					std::cerr << "cant read from the socket" << std::endl;
// 				}
// 				buff[r] = '\0';
// 				// Print the received data
// 				std::cout << YELLOW << buff << RESET<< std::endl;
// 				std::cout << "          \033[1;32m 🛠 ...... SERVER ON ...... 🛠\033[0m" << std::endl;

// 				// Parse the request
// 				Request req(buff);
// 				req.parseRequest();

// 				std::string path = req.getURL();
// 				// Loop over all the servers
// 				for (size_t i = 0; i < servers.size(); i++){
// 					// If the server's port matches the requested port, send a response
// 					if (servers[i].port == port){
// 						sendResponse(sock, req, servers[i]);
// 					}
// 				}
// 			}
//             // Close the socket
//             close(sock);
//             // Remove the socket from the master set
//             FD_CLR(sock, &master);
//         }
//     }
// }
	
   while(true){
    fd_set copy = master; // copy of the master set because select() modifies the set passed to it
    if (select(FD_SETSIZE, &copy, NULL, NULL, NULL) == -1) {
        std::cerr << "Select error: " << strerror(errno) << std::endl;
        return (-1);
    }

    for(int i = 0; i < FD_SETSIZE; i++){
        // Check if the socket is in the set
        if(FD_ISSET(i, &copy)){
            int sock = i;

            // If the socket is the server socket, it means there's a new connection
            if(sock == server){
                // Accept the new connection
                new_socket = accept(server, (struct sockaddr *)&address, &addrlen);

                // Check if accept() returned an error
                if (new_socket < 0) {
                    std::cerr << "Accept error: " << strerror(errno) << std::endl;
                    return (-1);
                }
				  // Make the new socket non-blocking
                int flags = fcntl(new_socket, F_GETFL, 0);
                if (flags == -1) {
                    std::cerr << "Fcntl error: " << strerror(errno) << std::endl;
                    return (-1);
                }
                flags |= O_NONBLOCK;
                if (fcntl(new_socket, F_SETFL, flags) == -1) {
                    std::cerr << "Fcntl error: " << strerror(errno) << std::endl;
                    return (-1);
                }
                // Add the new socket to the master set
                FD_SET(new_socket, &master);
            }
            else{
                // Receive data from the socket
                int  r = recv(sock, buff, BUFFER_SIZE, 0);
                // Check if recv() returned an error
                if( r < 0){
                    std::cerr << "cant read from the socket" << std::endl;
                }
                buff[r] = '\0';
                // Print the received data
                std::cout << YELLOW << buff << RESET<< std::endl;
                // Parse the request
                Request req(buff);
                req.parseRequest();

                std::string path = req.getURL();
                // Loop over all the servers
                for (size_t i = 0; i < servers.size(); i++){
                    // If the server's port matches the requested port, send a response
                    if (servers[i].port == port){
                        sendResponse(sock, req, servers[i]);
                    }
                }
                // Close the socket
                close(sock);
                // Remove the socket from the master set
                FD_CLR(sock, &master);
            }
        }
    }
}
	// close(server);
}