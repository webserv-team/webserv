    /* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 15:22:39 by hoigag            #+#    #+#             */
/*   Updated: 2024/03/22 18:01:01 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

bool isMethodAllowed(const std::string& method, std::vector<std::string>& allowedMethods)
{
    for (size_t i = 0; i < allowedMethods.size(); i++)
    {
        if (method == allowedMethods[i])
            return true;
    }
    return false;
}

Response::Response()
{
    this->response = "";
    this->httpVersion = "HTTP/1.1";
    this->contentType = "text/html";
    this->setStatusCode(200);
}

Location getCorrectLocation(std::string uri, std::vector<Location>& locations)
{
    while (true)
    {
        for (size_t i = 0; i < locations.size(); i++)
        {
            // std::cout << "location path == " << locations[i].path << std::endl;
            // std::cout << "uri == " << uri << std::endl;
            if (uri == locations[i].path)
            {
                std::cout << "found the right location" << std::endl;
                return locations[i];
            }
        }
        std::cout << std::endl;
        size_t pos = uri.find_last_of("/");
        if (pos == std::string::npos)
            break;
        uri = uri.substr(0, pos);
        if (uri.empty())
            break;
    }
    return locations[0];
}

std::string handleGetRequest(Request& req, Location& location, ConfigData& __unused conf)
{
    std::string uri = req.getURL();
    size_t pos = uri.find("?");
    std::string queryString;
    if (pos != std::string::npos)
    {
        queryString = uri.substr(pos + 1);
        uri = uri.substr(0, pos);
    }
    std::string requestedResource = location.root + uri;
    std::cout << "requested == " << requestedResource << std::endl;
    return "hello world";
}
Response::Response(Request &req, ConfigData &conf)
{
    this->httpVersion = "HTTP/1.1";
    this->contentType = "text/html";
    this->setStatusCode(200);
    std::string content;
    std::string header;
    std::string contentType;
    std::string queryString;
    std::string url = req.getURL();
    std::string resourceFullPath = conf.root;
    size_t pos = url.find("?");
    // if (req.getMethod() == "POST" && req.getContentType().find("multipart/form-data") != std::string::npos)
    //     uploadFiles(req);
    if (pos != std::string::npos)
    {
        queryString = url.substr(pos + 1);
        url = url.substr(0, pos);
    }
    Location location = getCorrectLocation(url, conf.locations);
    std::cout << location << std::endl; 
    if (!isMethodAllowed(req.getMethod(), location.methods))
    {
        std::cout << "method not allowed" << std::endl;
        this->setStatusCode(405);
        content = loadFile(conf.errorPages[this->getStatusCode()]);
    }
    else
    {
        if (req.getMethod() == "GET")
            content = handleGetRequest(req, location, conf);
    }
    
    this->setContentType(contentType);
    this->setContentLength(content.size());
    this->setBody(content);
    this->buildResponse();
    // std::cout << this->getResponseString() << std::endl; 
    // std::cout << this->body << std::endl;
}
// Response::Response(Request &req, ConfigData &conf)
// {
//     this->httpVersion = "HTTP/1.1";
//     this->contentType = "text/html";
//     this->setStatusCode(200);
//     std::string content;
//     std::string header;
//     std::string contentType;
//     std::string queryString;
//     std::string url = req.getURL();
//     std::string resourceFullPath = conf.root;
//     size_t pos = url.find("?");
//     // if (req.getMethod() == "POST" && req.getContentType().find("multipart/form-data") != std::string::npos)
//     //     uploadFiles(req);

//     // if (url == "/")
//     //     url = "/index.html";
//     if (pos != std::string::npos)
//     {
//         queryString = url.substr(pos + 1);
//         url = url.substr(0, pos);
//     }
//     Location location = getCorrectLocation(url, conf.locations);
//     std::cout << location << std::endl; 
//     if (!isMethodAllowed(req.getMethod(), location.methods))
//     {
//         std::cout << "method not allowed" << std::endl;
//         this->setStatusCode(405);
//         content = loadFile(conf.errorPages[this->getStatusCode()]);
//     }
//     else
//     {
//         if (location.path.empty())
//             resourceFullPath = conf.root + url;
//         else
//             resourceFullPath = location.root + url;

//         std::cout << "requested == " << resourceFullPath << std::endl;
//         //check if it's a file
//         if (access(resourceFullPath.c_str(), F_OK) != 0)    
//         {
//             std::cout << "file not found" << std::endl;
//             std::string copy = resourceFullPath;
//             while (true)
//             {
//                 size_t pos = copy.find_last_of("/");
//                 if (pos == std::string::npos)
//                     break;
//                 copy = copy.substr(0, pos);
//                 if (copy.empty())
//                     break;
//                 std::cout << "copy == " << copy << std::endl;
//                 if (access(copy.c_str(), F_OK) == 0)
//                 {
//                     std::cout << "found the right location" << std::endl;
//                     resourceFullPath = copy;
//                     break;
//                 }
//             }
//             if (location.autoindex == "on" && isDirectory(resourceFullPath))
//             {
//                 std::cout << "directory listing" << std::endl;
//                 content = directoryListing(resourceFullPath);
//             }
//             else
//             {
//                 std::cout << "not found" << std::endl;
//                 this->setStatusCode(403);
//                 content = loadFile(conf.errorPages[this->getStatusCode()]);
//             }
//         }
//         else if (access(resourceFullPath.c_str(), R_OK) != 0)
//         {
//             content = loadFile(conf.errorPages["404"]);
//             this->setStatusCode(403);
//         }
//         else if (isDirectory(resourceFullPath))
//         {
//             if (!location.index.empty())
//             {   
//                 std::string indexFile =  resourceFullPath + "/" + location.index;
//                 std::cout << "index == " << indexFile << std::endl;
//                 if (access(indexFile.c_str(), F_OK) == 0)
//                     content = loadFile(indexFile);
//                 else if (location.autoindex == "on")
//                     content = directoryListing(resourceFullPath);
//                 else
//                 { 
//                     this->setStatusCode(403);
//                     content = loadFile(conf.errorPages[this->getStatusCode()]);
//                 }
//             }
//             else if (location.autoindex == "on")
//                 content = directoryListing(resourceFullPath);
//             else
//             {
//                 this->setStatusCode(403);
//                 content = loadFile(conf.errorPages[this->getStatusCode()]);
//             }
//         }
//         else if (isSupportedCgiScript(resourceFullPath))
//         {
//             Cgi cgi(req);
//             content = cgi.executeScript(resourceFullPath);
//             size_t pos = content.find("\r\n\r\n");
//             if (pos != std::string::npos)
//             {
//                 content = content.substr(pos);
//                 header = content.substr(0, pos);
//             }
//         }
//         else
//         {
//             std::cout << "serving the file" << std::endl;
//             content = loadFile(resourceFullPath);
//         }
//         if (!isSupportedCgiScript(resourceFullPath))
//         {
//             std::string ext = getFileExtension(resourceFullPath);
//             contentType = this->mimes.getContentType(ext);
//         }
//         else if (isSupportedCgiScript(resourceFullPath))
//             contentType = getContentTypeFromCgiOutput(header);
//     }

    
//     this->setContentType(contentType);
//     this->setContentLength(content.size());
//     this->setBody(content);
//     this->buildResponse();
//     // std::cout << this->getResponseString() << std::endl; 
//     // std::cout << this->body << std::endl;
// }

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
    else if (status == 405)
        this->statusReason =  "Method Not Allowed";
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
    this->response = this->statusLine + "\r\n";
    this->response += "Content-Type: " + this->contentType + "\r\n";
    this->response += "Content-Length: " + this->contentLength + "\r\n";
    this->response += "\r\n";
    this->response += this->body;
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
