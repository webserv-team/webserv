/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 13:50:27 by hoigag            #+#    #+#             */
/*   Updated: 2024/03/30 19:17:10 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <dirent.h>
#include <ostream>
#include "Request.hpp"
#include "ServerConf.hpp"
#include "Cgi.hpp"
#include "helpers.hpp"
#include "Mimes.hpp"
#include "Request.hpp"
#include "responseHelpers.hpp"
#include "CgiParser.hpp"

#define GREEN "\033[1;32m"
#define RED "\033[1;31m"
#define BLUE "\033[1;34m"
#define RESET "\033[0m"

typedef struct s_data
{
	string statusCode;
	string body;
	string statusLine;
	string httpVersion;
	string statusReason;
	string contentType;
	map<string, string> headers;
	bool isRedirect;
} t_data;

class Response
{
	private:
		Response();
		Response(const Response& other);
		void buildResponse();
		std::string handleExistingFile(std::string path, Location& location);
		std::string handleRequest(Location& location);
		string loadErrorPages(string statusCode, string errorMessage);
		string handleDeleteRequest();
		string runCgi(Location& location);
		void formatResponse();
		string urlErrors();
		string getFullPath();
		
		Request req;
		ConfigData conf;
		Mimes mimes;
		string response;
		t_data data;
		string resourceFullPath;
	public:
		Response(Request &req, ConfigData &conf);
		~Response();
		t_data getData();
		std::string getResponseString();
};

std::ostream& operator<<(std::ostream& stream, Response& res);
string defaultError(string errorCode, string errorMessage);

#endif