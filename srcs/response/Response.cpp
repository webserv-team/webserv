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

string Response::getFullPath()
{
	string path;
	string url = req.getURL();
	if (url.find("?") != string::npos)
		url = url.substr(0, url.find("?"));
	Location loc = getMatchingLocation(url, conf);
	
	if (loc.alias.empty())
	{
		if (loc.root.empty())
			path = conf.root + url;
		else
			path = loc.root + url;
	}
	else
	{
		string alias = loc.alias;
		path = alias + url.substr(loc.path.size());
		if (path.back() != '/')
			path += "/";
	}
	return path;
}

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
	else if (transferEncodingChunked(req))
		return loadErrorPages("501", "Not Implemented");
	else if (loc.redirect.find(url) != loc.redirect.end())
	{
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

std::string Response::runCgi(Location& location)
{
	std::string content;
	Cgi cgi(this->req, location);
	CgiParser result =  cgi.executeCgiScript();
	std::string contentType = result.getContentType();
	std::string contentLength = result.getContentLength();
	std::string statusCode = result.getStatusCode();
	int code = atoi(statusCode.c_str());
	if (!contentType.empty() && this->mimes.isValidContentType(contentType))
	{
		this->data.headers["Content-Type"] = contentType;
		content = result.getBody();
		if (!contentLength.empty())
			this->data.headers["Content-Length"] = contentLength;
		else
			this->data.headers["Content-Length"] = itoa(content.size());
		if (code < 100 || code >= 600)
			this->data.statusCode = "200";
		else
			this->data.statusCode = statusCode;
		if (!result.getLocation().empty())
		{
			this->data.headers["Location"] = result.getLocation();
			this->data.statusCode = "302";
		}
	}
	else
	{	
		this->data.statusCode = "500";
		content = loadErrorPages(this->data.statusCode, "Internal Server Error");
	}
	return content;
}

std::string Response::handleExistingFile(std::string path, Location& location)
{
    
    std::string content;
    if (isDirectory(path))
    {
        if (path.back() != '/')
        {
			this->data.isRedirect = true;
            this->data.statusCode = "301";	
			this->data.headers["Location"] = this->req.getURL() + "/";
            return "redirecting to the directory\n";
        }
        std::string indexFile = path + "/" + location.index;	
        if (!location.index.empty())
        {	
			if (isFileExists(indexFile))
			{
				if (!location.cgiPath.empty() && isAllowdCgiExtension(indexFile))
					content = runCgi(location);
				else
				{
					
					content = loadFile(indexFile);
					this->data.headers["Content-Type"] = this->mimes.getContentType(indexFile);
				}
			}
			else
			{
				this->data.statusCode = "404";
				content = loadErrorPages(this->data.statusCode, "Not Found");
			}
        }
        else if (location.autoindex == "on")
        {
			try
			{
            	content = directoryListing(path);
			}
			catch(const std::exception& e)
			{
				this->data.statusCode = "403";
				content = loadErrorPages(this->data.statusCode, "Forbidden");
			}
        }
        else
        {
            this->data.statusCode = "403";
            content = loadErrorPages(this->data.statusCode, "Forbidden");
        }
    }
    else
    {
		if (!location.cgiPath.empty() && isAllowdCgiExtension(path))
			content = runCgi(location);
		else
		{
			try
			{
				content = loadFile(path);
				this->data.headers["Content-Type"] = this->mimes.getContentType(path);
			}
			catch(const std::exception& e)
			{
				this->data.statusCode = "403";
				content = loadErrorPages(this->data.statusCode, "Forbidden");
			}
		}
    }
    return content;
}

std::string Response::handleRequest(Location& location)
{
	// cerr << "handleRequest" << endl;
    std::string content;
	bool safe = true;
	string requestedResource = getFullPath();
	// std::cout << isAllowdCgiExtension(requestedResource) << std::endl;
	if (this->req.getMethod() == "POST" && this->req.getContentType().find("multipart/form-data") != std::string::npos && !isAllowdCgiExtension(requestedResource))
	{
		safe = uploadFiles(this->req, location);
		if (!safe)
		{
			this->data.statusCode = "500";
			std::cout << "Error uploading file" << std::endl;
			content = loadErrorPages(this->data.statusCode, "Internal Server Error");
		}
		else
		{
			this->data.statusCode = "201";
			content = loadErrorPages(this->data.statusCode, "file uploaded");
		}

	} 
    else if (isFileExists(requestedResource))
        content = this->handleExistingFile(requestedResource, location);
    else
    {

        this->data.statusCode = "404";
        content = loadErrorPages(this->data.statusCode, "Not Found");
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
	this->data.headers["Server"] = "webserv/1.0";
	this->data.headers["Connection"] = "keep-alive";
    this->data.statusCode = "200";
	this->data.isRedirect = false;
    this->formatResponse();
}

string Response::handleDeleteRequest()
{
	string content;

	if (isDirectory(resourceFullPath))
	{
		if (resourceFullPath.back() != '/')
		{
			data.statusCode = "409";
			content = loadErrorPages(data.statusCode, "Conflict");
		}
		else
		{
			if (removeDir(resourceFullPath))
				data.statusCode = "204";
			else
			{
				data.statusCode = "403";
				content = loadErrorPages(data.statusCode, "Forbidden");
			}
		}
	}
	else if (isFileExists(resourceFullPath))
	{
		remove(resourceFullPath.c_str());
		data.statusCode = "204";
	}
	else
	{
		this->data.statusCode = "404";
		content = loadErrorPages(this->data.statusCode, "Not Found");
	}
	return content;
}

void Response::formatResponse()
{
    std::string content;
    std::string url = req.getURL();
	this->resourceFullPath = getFullPath();
    Location location = getMatchingLocation(url, conf);
	content = urlErrors();
	if (content.empty() && data.isRedirect == false)
    {
        if (req.getMethod() == "GET" || req.getMethod() == "POST")
        	content = handleRequest(location);
		else if (req.getMethod() == "DELETE")
			content = handleDeleteRequest();
    }
	if (!data.isRedirect)
	{
		this->data.headers["Content-Length"] = itoa(content.size());
		this->data.body = content;
	}
	data.headers["Content-Length"] = itoa(content.size());
    this->buildResponse();
}

Response::~Response(){}

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
	return stream;
}


string Response::loadErrorPages(string statusCode, string errorMessage)
{
	map <string, string>::iterator it = conf.errorPages.find(statusCode);
	string content;

	data.statusCode = statusCode;
	if (it != conf.errorPages.end())
	{
		if (isFileExists(it->second))
		{
			if (access(it->second.c_str(), R_OK) == -1)
				content = defaultError("403", "Forbidden");
			else
				content = loadFile(it->second);
		}
		else
			content = defaultError(statusCode, errorMessage);
	}
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
	if (this->response.empty())
		return loadErrorPages("500", "Internal Server Error");
	return this->response;
}

t_data Response::getData()
{
	return this->data;
}