/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emohamed <emohamed@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 10:16:59 by ogorfti           #+#    #+#             */
/*   Updated: 2024/01/27 14:33:10 by emohamed         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include "../parsing/Request.hpp"
#include "../parsing/ServerConf.hpp"
const int PORT = 8080;
const int BUFFER_SIZE = 1024;
#define GREEN "\033[1;32m"
#define RED "\033[1;31m"
#define RESET "\033[0m"

using namespace std;
//sigpipe?
string getContentType(const string& filePath)
{
	string extension = filePath.substr(filePath.find_last_of(".") + 1);
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
	string errorResponse = "HTTP/1.1 " + to_string(statusCode) + " Internal Server Error\r\n";
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
					"<h1>Error " + to_string(statusCode) + "</h1>"
				"</div>"
			"</div>"
		"</body>"
		"</html>";
	if (send(socket, errorResponse.c_str(), errorResponse.length(), 0) < 0)
		cerr << RED << "Error sending error response: " << strerror(errno) << RESET << endl;
}

void error500(int socket, string& filePath)
{
	ifstream file(filePath);
	if (!file.is_open())
	{
		cerr << "Error page not found" << endl;
		defaultError(socket, 500);
		return ;
	}
	stringstream fileContent;
	fileContent << file.rdbuf();
	string content = fileContent.str();
	file.close();
	string response = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\nContent-Length: ";
	response += to_string(content.length()) + "\r\n\r\n" + content;
	if (send(socket, response.c_str(), response.length(), 0) < 0)
	{
		cerr << "Send error: " << strerror(errno) << endl;
		return ;
	}
}

int fileExists(string& url, ConfigData& server)
{
	for (size_t i = 0; i < server.locations.size(); i++)
	{
		if (url.find(server.locations[i].path) == 0)
		{
			string filePath = server.documentRoot + server.locations[i].root + url.substr(server.locations[i].path.length());
			ifstream file(filePath);
			if (file.is_open())
			{
				file.close();
				return (1);
			}
			else
				return (-1);
		}
	}
	return 0;
}

void sendResponse(int socket, Request& request, ConfigData& server)
{
	string filePath = server.documentRoot;
	if (request.getURL() == "/")
		filePath += "/index.html";
	else
		filePath += request.getURL();
	std::cout << "filePath: " << filePath << std::endl;
	// Try to open the file
	ifstream file(filePath);
	if (file.is_open())
	{
		// Read the file into a string
		stringstream fileContent;
		fileContent << file.rdbuf();
		string content = fileContent.str();
		file.close();
		
		// Send the response
		string response = "HTTP/1.1 200 OK\r\nContent-Type: " + getContentType(filePath) + "\r\nContent-Length: ";
		response += to_string(content.length()) + "\r\n\r\n" + content;
		if (send(socket, response.c_str(), response.length(), 0) < 0)
		{
			cerr << RED << "Send error: " << strerror(errno) << RESET << endl;
			error500(socket, server.errorPages["server_error"]);
			return;
		}	
		cout << GREEN << "Response sent" << RESET << "   URL:" << request.getURL() << endl;
	}
	else
	{
		string tmp = server.errorPages["not_found"];
		file.open(tmp);
		if (!file.is_open())
		{
			cerr << RED << "Error page not found: " << strerror(errno) << RESET << endl;
			error500(socket, server.errorPages["server_error"]);
			return ;
		}
		stringstream fileContent;
		fileContent << file.rdbuf();
		string content = fileContent.str();
		file.close();
		string response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: ";
		response += to_string(content.length()) + "\r\n\r\n" + content;
		
		if (send(socket, response.c_str(), response.length(), 0) < 0)
		{
			cerr << RED << "Send error: " << strerror(errno) << RESET << endl;
			error500(socket, server.errorPages["server_error"]);
			return ;
		}
	}
}

// socket -> bind -> listen -> accept -> recv -> send -> close
// socket is like a virtual divice likaysam7 ljuj computers ycommunicatiw m3a ba3dyatem
int main()
{
	int server_fd;

	// socket(domain, type, protocol) : domain = AF_INET (IPv4), type = SOCK_STREAM (TCP), protocol = 0 (IP)
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		cerr << "Socket creation error: " << strerror(errno) << endl;
		return (-1);
	}
	
	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		cerr << "Setsockopt error: " << strerror(errno) << endl;
		return (-1);
	}
	struct sockaddr_in address;

	address.sin_family = AF_INET; // IPv4
	address.sin_addr.s_addr = htonl(INADDR_ANY); // htonl() converts a long integer (e.g. address) to a network representation
	address.sin_port = htons(PORT); // htons() converts a short integer (e.g. port) to a network representation

	bind(server_fd, (struct sockaddr *)&address, sizeof(address));
	// if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	// {
	// 	cerr << "Bind error: " << strerror(errno) << endl;
	// 	return (-1);
	// }

	// listen(socket, backlog) : backlog = 3 (max number of pending connections)
	if (listen(server_fd, 3) < 0)
	{
		cerr << "Listen error: " << strerror(errno) << endl;
		return (-1);
	}

	socklen_t addrlen = sizeof(address);
	char buffer[BUFFER_SIZE];
	int new_socket, valread;
	
	ServerConf tmp("../default.toml");
	vector<ConfigData> servers = tmp.getServers();
	signal(SIGPIPE, SIG_IGN); // this mean ignore the signal SIGPIPE because 
	while (true)
	{
		cout << "\n+++++++ Waiting for new connection ++++++++" << endl;
		new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
		if (new_socket < 0)
		{
			cerr << "Accept error: " << strerror(errno) << endl;
			return (-1);
		}
		if ((valread = recv(new_socket, buffer, BUFFER_SIZE, 0)) < 0)
		{
			cerr << "Recv error: " << strerror(errno) << endl;
			return (-1);
		}
		buffer[valread] = '\0';
		cout << "Request received: " << buffer << endl;
	   
		Request request(buffer);
		request.parseRequest();		

		for (size_t i = 0; i < servers.size(); i++)
		{
			if (servers[i].port == 8080)
			{
				// cout << "Server found: " << servers[i].hostName << endl;
				sendResponse(new_socket, request, servers[i]);
				// break;
			}
		}
		close(new_socket);
	}
	close(server_fd);
}

