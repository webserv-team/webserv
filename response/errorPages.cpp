/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ogorfti <ogorfti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/31 15:48:18 by ogorfti           #+#    #+#             */
/*   Updated: 2024/01/31 15:55:22 by ogorfti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

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
