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

// std::ostream& operator<<(std::ostream& stream, Response& res)
// {
// 	(void)res;
// 	std::time_t currentTime;
// 	std::time(&currentTime);
// 	char buffer[80];
// 	std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&currentTime));
// 	// std::string color = (res.getStatusCode() == "200") ? GREEN : RED;
// 	// stream << BLUE << "[RESPONSE " << buffer << "] " << RESET << color << res.getStatusLine() << RESET << std::endl;
// 	// stream << "++++++++++++++++++ Response +++++++++++++++++++++++++\n";
// 	// stream << res.getStatusLine() << std::endl;
// 	// stream << res.getContentType() << std::endl;
// 	// stream << res.getContentLength() << std::endl;
// 	// stream  << std::endl;
// 	// stream  << res.getBody() << std::endl;
// 	// stream << "++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
// 	return stream;
// }

/*-------------------- Member functions --------------------*/

string getStatusReason(string& status)
{
	if (status == "200")
		return "OK";
	else if (status == "400")
		return "Bad Request";
	else if (status == "403")
		return "Forbidden";
	else if (status == "404")
		return "Not Found";
	else if (status == "405")
		return "Method Not Allowed";
	else if (status == "413")
		return "Request Entity Too Large";
	else if (status == "414")
		return "Request-URI Too Long";
	else if (status == "501")
		return "Not Implemented";
	else if (status == "301")
		return "Moved Permanently";
	else
		return "";
}

Location getMatchingLocation(const string& url, ConfigData& conf)
{
	Location* longestMatch = nullptr;
	
	for (size_t i = 0; i < conf.locations.size(); i++)
	{
		Location* loc = &conf.locations[i];
		if (url.find(loc->path) == 0)
		{
			if (longestMatch == nullptr || loc->path.size() > longestMatch->path.size())
				longestMatch = loc;
		}
	}
	return *longestMatch;
}

long long toBytes(string& bodyLimit)
{
	char* end;
	long long value = strtol(bodyLimit.c_str(), &end, 10);
	char unit = tolower(*end);

	switch (unit)
	{
		case 'k':
			value *= 1024;
			break;
		case 'm':
			value *= 1024 * 1024;
			break;
		case 'g':
			value *= 1024 * 1024 * 1024;
			break;
		case 'b':
		default:
			break;
	}

	return value;
}

// when i submit a file its got downloaded!! and its uploaded to the server
bool bodyLimitExceeded(Request& req, Location& loc)
{
	// cerr << "req.getBody().size() == " << req.getBody().size() << endl;
	// cerr << "toBytes(loc.bodyLimit) == " << toBytes(loc.bodyLimit) << endl;
	if (toBytes(loc.bodyLimit) < (long long)req.getBody().size())
		return true;
	return false;
}

bool chrURL(string& url)
{
	string str = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-./:;=@[]!&'*+,%";

	for (size_t i = 0; i < url.size(); i++)
	{
		if (str.find(url[i]) == string::npos)
			return true;
	}
	return false;
}

string loadErrorPages(ConfigData& conf, t_data& data, string statusCode, string errorMessage)
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

// if tansfer-encoding is exist and its value is different than chunked
bool transferEncodingChunked(Request& req)
{
	map<string, string> headers = req.getHeaders();
	if (headers.find("Transfer-Encoding") != headers.end() && headers["Transfer-Encoding"] != "chunked")
	{
		cerr << RED << "Transfer-Encoding: " << headers["Transfer-Encoding"] << RESET << endl;	
		return true;
	}
	return false;
}

// if transfer-encoding is not exist and content-length is not exist the method is POST
bool verifyPostReq(Request& req)
{
	if (req.getMethod() == "POST")
	{
		map<string, string> headers = req.getHeaders();
		if (headers.find("Content-Length") == headers.end() && headers.find("Transfer-Encoding") == headers.end())
			return true;
	}
	return false;
}

string	urlErrors(Request& req, ConfigData& conf, t_data& data)
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
		return loadErrorPages(conf, data, "405", "Method Not Allowed");
	else if (chrURL(url) || verifyPostReq(req))
		return loadErrorPages(conf, data, "400", "Bad Request");
	else if (bodyLimitExceeded(req, loc))
		return loadErrorPages(conf, data, "413", "Request Entity Too Large");
	else if (url.length() > 2048)
		return loadErrorPages(conf, data, "414", "Request-URI Too Long");
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

Response::Response(Request &req, ConfigData &conf)
{
	cerr << BLUE << req << RESET << endl;
	data.httpVersion = "HTTP/1.1";
	data.headers["Content-Type"] = "text/html";
	data.statusCode = "200";
	data.isRedirect = false;
	std::string content;
	std::string header;
	std::string contentType;
	std::string resourceFullPath = conf.root;
	std::string url = req.getURL();
	Location loc = getMatchingLocation(url, conf);
	size_t pos = url.find("?");
	if (req.getMethod() == "POST" && req.getContentType().find("multipart/form-data") != std::string::npos)
		uploadFiles(req);
	
	// get the full path
	if (url == "/")
		url = "/index.html";
	if (pos != std::string::npos)
		resourceFullPath += url.substr(0, pos);
	else
		resourceFullPath += url;
	// cerr << BLUE << "resourceFullPath == " << resourceFullPath << RESET << endl;
	
	content = urlErrors(req, conf, data);
	if (content.size() != 0 || data.isRedirect)
	{
		// this->setStatusCode(405);
		// data.statusCode = "405";
	}
	else if (access(resourceFullPath.c_str(), F_OK) != 0)    
	{
		data.statusCode = "404";
		content = loadFile(conf.errorPages["404"]);
	}
	else if (access(resourceFullPath.c_str(), R_OK) != 0)
	{
		content = loadFile(conf.errorPages["403"]);
		data.statusCode = "403";
	}
	else if (isDirectory(resourceFullPath))
		content = directoryListing(resourceFullPath);
	else if (isSupportedCgiScript(resourceFullPath))
	{
		Cgi cgi(req);
		content = cgi.executeScript(resourceFullPath);
		size_t pos = content.find("\r\n\r\n");
		if (pos != std::string::npos)
		{
			content = content.substr(pos);
			header = content.substr(0, pos);
		}
	}
	else
		content = loadFile(conf.root + url);
	
	// get content type
	if (!isSupportedCgiScript(url))
	{
		std::string ext = getFileExtension(url);
		contentType = this->mimes.getContentType(ext);
	}
	else if (isSupportedCgiScript(url))
		contentType = getContentTypeFromCgiOutput(header);
	
	// data.contentType = contentType;
	// data.contentLength = std::to_string(content.size());
	data.headers["Content-Type"] = contentType;
	data.headers["Content-Length"] = std::to_string(content.size());
	data.body = content;
	this->buildResponse(data);
}

void Response::buildResponse(t_data& data)
{
	this->response = data.httpVersion + " " + data.statusCode + " ";
	this->response += getStatusReason(data.statusCode) + "\r\n";
	// this->response += "Content-Type: " + data.contentType + "\r\n";
	// this->response += "Content-Length: " + data.contentLength + "\r\n";
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