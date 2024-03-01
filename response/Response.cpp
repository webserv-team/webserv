/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ogorfti <ogorfti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/31 12:47:54 by ogorfti           #+#    #+#             */
/*   Updated: 2024/02/12 14:25:40 by ogorfti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

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

string extractFilePath(const string& url, ConfigData& server)
{
	string filePath = server.root;
	
	for (size_t i = 0; i < server.locations.size(); i++)
	{
		std::string path = server.locations[i].path;
		// if (url.find(path) == 0 && (url.length() == path.length() || url[path.length()] == '/'))
		if (url.find(server.locations[i].path) == 0)
		{
			// cerr << GREEN << "ROOT: " <<  server.locations[i].root << RESET << endl;
			filePath = server.locations[i].root;
			
			if (url == "/")
				filePath += "/index.html";
			else
			{
				filePath += url.substr(server.locations[i].path.length() - 1);
			}
			break;
		}
	}
	return filePath;
}

void Response::sendResponse(int socket, Request& request, ConfigData& server)
{
	// if (request.getMethod() == "POST")
	// {
	// 	cerr << RED << "POST method not implemented" << RESET << endl;
	// 	error500(socket, server.errorPages["server_error"]);
	// 	return ;
	// }
	string filePath = extractFilePath(request.getURL(), server);
	cerr << RED << "filePath: " << extractFilePath(request.getURL(), server) << RESET << endl;

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
		cout << GREEN << "Response sent" << RESET << endl;
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
