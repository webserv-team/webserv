    /* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ogorfti <ogorfti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 15:22:39 by hoigag            #+#    #+#             */
/*   Updated: 2024/03/24 23:28:57 by ogorfti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

string	Response::urlErrors()
{
	Location loc = getMatchingLocation(req.getURL(), conf);
	string method = req.getMethod();
	string url = req.getURL();
	bool methodFound = false;
	string content;
	
	for (size_t i = 0; i < loc.methods.size(); i++)
	{
		if (loc.methods[i] == method)
			methodFound = true;
	}
	if (!methodFound)
		return loadErrorPages("405", "Method Not Allowed");
	else if (chrURL(url) || verifyPostReq(req))
		return loadErrorPages("400", "Bad Request");
	else if (bodyLimitExceeded(req, loc))
		return loadErrorPages("413", "Request Entity Too Large");
	else if (url.length() > 2048)
		return loadErrorPages("414", "Request-URI Too Long");
	// else if (transferEncodingChunked(req))
	// 	return loadErrorPages(conf, data, "501", "Not Implemented");
	else if (loc.redirect.find(url) != loc.redirect.end())
	{
		cerr << RED << "redirecting from " << url << " to " << loc.redirect[url] << RESET << endl;
		data.isRedirect = true;
		data.statusCode = "301";
		data.headers["Location"] = loc.redirect[url];
	}
	return content;
}

Response::Response()
{
    this->response = "";
    this->data.httpVersion = "HTTP/1.1";
	this->data.contentType = "text/html";
	this->data.statusCode = "200";
}

std::string Response::handleExistingFile(std::string path, Location& location)
{
    
    std::string content;
    if (isDirectory(path))
    {
			// std::cout << "url === " << this->req.getURL() << std::endl;
        if (path.back() != '/')
        {
			std::cout << "redirecting to the directory" << std::endl;
			this->data.isRedirect = true;
            this->data.statusCode = "301";
			this->data.headers["Location"] = this->req.getURL() + "/";
            return "redirecting to the directory\n";
        }
        std::string indexFile = path + "/" + location.index;
        if (!location.index.empty() && isFileExists(indexFile))
        {
            std::cout << "serving the index file" << std::endl;
            content = loadFile(indexFile);
        }
        else if (location.autoindex == "on")
        {
            std::cout << "directory listing" << std::endl;
            content = directoryListing(path);
        }
        else
        {
            this->data.statusCode = "403";
            content = loadFile(this->conf.errorPages[this->data.statusCode]);
        }
    }
    else
    {
        std::cout << "serving the file" << std::endl;
        content = loadFile(path);
		this->data.headers["Content-Type"] = this->mimes.getContentType(path);
    }
    return content;
}

std::string Response::handleGetRequest(Location& location)
{
    std::string uri = this->req.getURL();
    std::string content;
    size_t pos = uri.find("?");
    std::string queryString;
    if (pos != std::string::npos)
    {
        queryString = uri.substr(pos + 1);
        uri = uri.substr(0, pos);
    }
    std::string requestedResource = location.root + uri;
    // std::cout << "requested == " << requestedResource << std::endl;
    if (isFileExists(requestedResource))
        content = this->handleExistingFile(requestedResource, location);
    else
    {
        this->data.statusCode = "404";
        content = loadFile(conf.errorPages[this->data.statusCode]);
		this->data.headers["Content-Type"] = this->mimes.getContentType(requestedResource);
    }
    return content;
}
Response::Response(Request &req, ConfigData &conf)
{
    this->req = req;
    this->conf = conf;
    this->data.httpVersion = "HTTP/1.1";
    this->data.headers["Content-Type"] = "text/html";
    this->data.statusCode = "200";
	this->data.isRedirect = false;
    this->formatResponse();
}

void Response::formatResponse()
{
    std::string content;
    std::string header;
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
    Location location = getMatchingLocation(url, conf);
	content = urlErrors();
    // std::cout << location << std::endl; 
	if (content.empty() && data.isRedirect == false)
    {
        if (req.getMethod() == "GET")
            content = handleGetRequest(location);
    }
	if (!data.isRedirect)
	{
		this->data.headers["Content-Length"] = content.size();
		this->data.body = content;
	}
    this->buildResponse();
}
/*-------------------- Constructors --------------------*/


Response::~Response(){}

// Response::Response(const Response& other){}

// Response& Response::operator=(const Response& other){}

/*-------------------- Tmp functions --------------------*/

ostream& operator<<(ostream& os, Location& loc)
{
	os << "Location: " << loc.path << endl;
	os << "Root: " << loc.root << endl;
	os << "Index: " << loc.index << endl;
	os << "Methods: ";
	for (size_t i = 0; i < loc.methods.size(); i++)
		os << loc.methods[i] << " ";
	os << endl;
	os << "Cgi: " << loc.cgiPath << endl;
	os << "Autoindex: " << loc.autoindex << endl;
	os << "Upload: " << loc.uploadPath << endl;
	os << "MaxBody: " << loc.bodyLimit << endl;
	return os;
}

std::ostream& operator<<(std::ostream& stream, Response& res)
{
	(void)res;
	std::time_t currentTime;
	std::time(&currentTime);
	char buffer[80];
	std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&currentTime));
	t_data data = res.getData();
	std::string color = (data.statusCode == "200") ? GREEN : RED;
	stream << BLUE << "[RESPONSE " << buffer << "] " << RESET << color << 
	data.httpVersion << " " << data.statusCode << " " << getStatusReason(data.statusCode) << RESET << std::endl;
	// stream << "++++++++++++++++++ Response +++++++++++++++++++++++++\n";
	// stream << res.getStatusLine() << std::endl;
	// stream << res.getContentType() << std::endl;
	// stream << res.getContentLength() << std::endl;
	// stream  << std::endl;
	// stream  << res.getBody() << std::endl;
	// stream << "++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
	return stream;
}

/*-------------------- Member functions --------------------*/



string Response::loadErrorPages(string statusCode, string errorMessage)
{
	map <string, string>::iterator it = conf.errorPages.find(statusCode);
	string content;

	data.statusCode = statusCode;
	if (it != conf.errorPages.end())
		content = loadFile(it->second);
	else
		content = defaultError(statusCode, errorMessage);
	return content;
}

void Response::buildResponse()
{
	this->response = data.httpVersion + " " + data.statusCode + " ";
	this->response += getStatusReason(data.statusCode) + "\r\n";
	for (map<string, string>::iterator it = data.headers.begin(); it != data.headers.end(); it++)
		this->response += it->first + ": " + it->second + "\r\n";
	this->response += "\r\n";
	this->response += data.body;
}

/*-------------------- Getters --------------------*/

std::string Response::getResponseString()
{
	return this->response;
}

t_data Response::getData()
{
	return this->data;
}