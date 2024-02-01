/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 13:50:27 by hoigag            #+#    #+#             */
/*   Updated: 2024/02/01 17:59:41 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <ostream>
class Response
{
	public:
		Response();
		~Response();
		// Response(const Response& other);
		// Response& operator=(const Response& other);
		void setStatusCode(short status);
		void setContentType(std::string contentType);
		void setContentLength(int contentLength);
		void setBody(std::string body);
		void sendIt(int sock);
		void setStatusLine();
		
		std::string getStatusCode();
		std::string getContentType();
		std::string getContentLength();
		std::string getBody();
		std::string getStatusLine();
	private:
		std::string delim;
		std::string statusCode;
		std::string contentType;
		std::string contentLength;
		std::string body;
		std::string statusReason;
		std::string httpVersion;
		std::string response;
		std::string statusLine;
		void setStatusReason(short status);
};

std::ostream& operator<<(std::ostream& stream, Response& res);

#endif
