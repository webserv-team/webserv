/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 13:50:27 by hoigag            #+#    #+#             */
/*   Updated: 2024/03/24 18:21:39 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <ostream>
#include "Request.hpp"
#include "ServerConf.hpp"
#include "Cgi.hpp"
#include "helpers.hpp"
#include "Mimes.hpp"


#define GREEN "\033[1;32m"
#define RED "\033[1;31m"
#define BLUE "\033[1;34m"
#define RESET "\033[0m"

class Response
{
	public:
		Response();
		Response(Request &req, ConfigData &conf);
		~Response();
		// Response(const Response& other);
		// Response& operator=(const Response& other);
		void setStatusCode(short status);
		void setContentType(std::string contentType);
		void setContentLength(int contentLength);
		void setBody(std::string body);
		void sendIt(int sock);
		void setStatusLine();
		void buildResponse();
		int getResponseLength();
		void formatResponse();
		std::string handleExistingFile(std::string path, Location& location);
		std::string handleNonExistingFile(std::string path, Location& location);
		std::string handleGetRequest(Location& location);

		std::string getResponseString();
		std::string getStatusCode();
		std::string getContentType();
		std::string getContentLength();
		std::string getBody();
		std::string getStatusLine();
	private:
		std::string statusCode;
		std::string contentType;
		std::string contentLength;
		std::string body;
		std::string statusReason;
		std::string httpVersion;
		std::string response;
		std::string statusLine;
		Request req;
		ConfigData conf;
		Mimes mimes;
		void setStatusReason(short status);
};

std::ostream& operator<<(std::ostream& stream, Response& res);

#endif


