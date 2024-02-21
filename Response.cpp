/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 15:22:39 by hoigag            #+#    #+#             */
/*   Updated: 2024/02/20 17:32:38 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response()
{
    this->response = "";
    this->httpVersion = "HTTP/1.1";
    this->contentType = "text/html";
    this->delim = "\r\n";
    this->setStatusCode(200);
}
Response::~Response(){}

// Response::Response(const Response& other)
// {

// }

// Response& Response::operator=(const Response& other)
// {

// }

void Response::setStatusCode(short status)
{
    this->statusCode = std::to_string(status);
    this->setStatusReason(status);
    this->setStatusLine();
}

void Response::setContentType(std::string contentType)
{
    this->contentType = contentType;
}

void Response::setContentLength(int contentLength)
{
    this->contentLength = std::to_string(contentLength);
}

void Response::setBody(std::string body)
{
    this->body = body;
}

void Response::setStatusReason(short status)
{
    if (status == 200)
        this->statusReason =  "OK";
    else if (status == 400)
        this->statusReason =  "Bad Request";
    else if (status == 403)
        this->statusReason =  "Forbidden";
    else if (status == 404)
        this->statusReason =  "Not Found";
    else
        this->statusReason = "";
}

void Response::setStatusLine()
{
    this->statusLine = this->httpVersion + " " + this->statusCode + " " + this->statusReason;
}

void Response::sendIt(int sock)
{
    this->response = this->statusLine + this->delim;
    this->response += "Content-Type: " + this->contentType + this->delim;
    this->response += "Content-Length: " + this->contentLength + this->delim;
    this->response += this->delim;
    this->response += this->body;
    if (send(sock, this->response.c_str(), response.length(), 0) < 0)
	{
		std::cerr << "error: Could not send data" << std::endl;
		exit(1);
	}
    // close(sock);
}
std::string Response::getStatusCode()
{
    return this->statusCode;
}
std::string Response::getContentType()
{
    return this->contentType;
}
std::string Response::getContentLength()
{
    return this->contentLength;
}
std::string Response::getBody()
{
    return this->body;
}
std::string Response::getStatusLine()
{
    return this->statusLine;
}
std::ostream& operator<<(std::ostream& stream, Response& res)
{
    stream << "++++++++++++++++++ Response +++++++++++++++++++++++++\n";
    stream << res.getStatusLine() << std::endl;
    stream << res.getContentType() << std::endl;
    stream << res.getContentLength() << std::endl;
    stream  << std::endl;
    stream  << res.getBody() << std::endl;
    stream << "++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
    return stream;
}