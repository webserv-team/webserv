/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 15:22:39 by hoigag            #+#    #+#             */
/*   Updated: 2024/02/28 10:54:46 by hoigag           ###   ########.fr       */
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

const char * Response::getResponseString()
{
    return this->response.c_str();
}

void Response::setStatusLine()
{
    this->statusLine = this->httpVersion + " " + this->statusCode + " " + this->statusReason;
}

void Response::buildResponse()
{
    this->response = this->statusLine + this->delim;
    this->response += "Content-Type: " + this->contentType + this->delim;
    this->response += "Content-Length: " + this->contentLength + this->delim;
    this->response += this->delim;
    this->response += this->body;
}

void Response::sendIt(int sock)
{
    int totalDataSent = 0;
    int dataSent = 0;
    int responseLength = this->response.length();
    dataSent = send(sock, this->response.c_str() + dataSent, responseLength - totalDataSent, 0);
    if (dataSent < 0)
        std::cerr << "error: Could not send data" << std::endl;
    else
        totalDataSent += dataSent;
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

int Response::getResponseLength()
{
    return this->response.length();
}

