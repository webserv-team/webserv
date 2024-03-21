/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 15:22:39 by hoigag            #+#    #+#             */
/*   Updated: 2024/03/19 15:52:26 by hoigag           ###   ########.fr       */
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

std::string Response::getResponseString()
{
    return this->response;
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
    std::time_t currentTime;
    std::time(&currentTime);
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&currentTime));
    std::string color = (res.getStatusCode() == "200") ? GREEN : RED;
	stream << BLUE << "[RESPONSE " << buffer << "] " << RESET << color << res.getStatusLine() << RESET << std::endl;
    // stream << "++++++++++++++++++ Response +++++++++++++++++++++++++\n";
    // stream << res.getStatusLine() << std::endl;
    // stream << res.getContentType() << std::endl;
    // stream << res.getContentLength() << std::endl;
    // stream  << std::endl;
    // stream  << res.getBody() << std::endl;
    // stream << "++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
    return stream;
}

int Response::getResponseLength()
{
    return this->response.length();
}

// void	Response::formResponse(Request& req)
// {
//     Response response;
//     std::string content;
//     std::string header;
//     std::string contentType;
//     ConfigData conf = this->getServer(req.getPort()).getConfData();
//     std::string resourceFullPath = conf.documentRoot;
//     std::string url = req.getURL();
//     size_t pos = url.find("?");
//     if (req.getMethod() == "POST" && req.getContentType().find("multipart/form-data") != string::npos)
//         uploadFiles(req);
//     if (url == "/")
//         url = "/index.html";
//     if (pos != std::string::npos)
//         resourceFullPath += url.substr(0, pos);
//     else
//         resourceFullPath += url;
//     if (access(resourceFullPath.c_str(), F_OK) != 0)    
//     {
//         content = loadFile(conf.errorPages["not_found"]);
//         response.setStatusCode(404);
//     }
//     else if (access(resourceFullPath.c_str(), R_OK) != 0)
//     {
//         content = loadFile(conf.errorPages["forbidden"]);
//         response.setStatusCode(403);
//     }
//     else if (isDirectory(resourceFullPath))
//         content = this->directoryListing(resourceFullPath);
//     else if (isSupportedCgiScript(resourceFullPath))
//     {
//         Cgi cgi(req);
//         content = cgi.executeScript(resourceFullPath);
//         size_t pos = content.find("\r\n\r\n");
//         if (pos != std::string::npos)
//         {
//             content = content.substr(pos);
//             header = content.substr(0, pos);
//         }
//     }
//     else
//         content = loadFile(conf.documentRoot + url);
//     if (!isSupportedCgiScript(url))
//     {
//         std::string ext = getFileExtension(url);
//         contentType = mimes.getContentType(ext);
//     }
//     else if (isSupportedCgiScript(url))
//         contentType = getContentTypeFromCgiOutput(header);
//     response.setContentType(contentType);
//     response.setContentLength(content.size());
//     response.setBody(content);
//     response.buildResponse();
//     return response;
// }
