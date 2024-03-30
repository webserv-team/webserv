/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/01 16:18:37 by ogorfti           #+#    #+#             */
/*   Updated: 2024/03/30 20:30:54 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

/*-------------------- Constructors --------------------*/

Request::Request(Request &request)
{
	*this = request;
}

Request::Request() {
	this->request_ = "";
	this->method_ = "";
	this->url_ = "";
	this->protocol_ = "";
	this->body_ = "";
	this->lines_ = std::vector<std::string>();
	this->headers_ = std::map<std::string, std::string>();
	
}

Request &Request::operator=(Request &request)
{
	this->request_ = request.request_;
	this->lines_ = request.lines_;
	this->method_ = request.method_;
	this->url_ = request.url_;
	this->protocol_ = request.protocol_;
	this->headers_ = request.headers_;
	this->body_ = request.body_;
	return *this;
}

Request::~Request() {}

/*-------------------- Member functions --------------------*/

void Request::splitLines()
{
	size_t begin = 0;
	size_t end = 0;

	while ((end = request_.find("\r\n", begin)) != std::string::npos)
	{
		this->lines_.push_back(request_.substr(begin, end - begin));
		begin = end + 2;
	}
}

void Request::parseFirstLine()
{
	if (lines_.empty())
		return;
	std::string firstLine = lines_[0];
	size_t methodEnd = firstLine.find(" ");
	size_t urlEnd = firstLine.find(" ", methodEnd + 1);

	this->method_ = firstLine.substr(0, methodEnd);
	this->url_ = firstLine.substr(methodEnd + 1, urlEnd - methodEnd - 1);
	this->protocol_ = firstLine.substr(urlEnd + 1);
}

void Request::parseHeaders()
{
	size_t pos = 0;
	for (size_t i = 1; i < this->lines_.size(); i++)
	{
		pos = lines_[i].find(":");
		if (lines_[i].empty()) // means that body starts
			isBody_ = true;
		if (pos != std::string::npos && !isBody_)
		{
			std::string key = lines_[i].substr(0, pos);
			std::string value = lines_[i].substr(pos + 2);
			headers_[key] = value;
		}
	}
}

void Request::parseBody()
{
	size_t pos = request_.find("\r\n\r\n");
	this->body_ = request_.substr(pos + 4);
}

void Request::parseMultipart()
{
	size_t Bpos = headers_["Content-Type"].find("boundary=");
	string boundary = headers_["Content-Type"].substr(Bpos + 9);
	
	vector <string> parts;
 	size_t pos = 0, end;

	while ((end = body_.find("--" + boundary, pos)) != string::npos)
	{
		string part = body_.substr(pos, end - pos);
		if (end != 0)
			parts.push_back(part);
		pos = end + boundary.length() + 4; // 4 for "\r\n--"
	}
	for (size_t i = 0; i < parts.size(); i++)
	{
		s_tuple tmp;
		
		size_t pos1 = parts[i].find("name=\"");
		if (pos1 != string::npos)
		{
			size_t pos2 = parts[i].find("\"", pos1 + 6);
			tmp.name = parts[i].substr(pos1 + 6, pos2 - pos1 - 6); // 6 for "name=\""
		}
		size_t pos3 = parts[i].find("filename=\"");
		if (pos3 != string::npos)
		{
			size_t pos4 = parts[i].find("\"", pos3 + 10);
			tmp.fileName = parts[i].substr(pos3 + 10, pos4 - pos3 - 10);
		}
		size_t pos5 = parts[i].find("\r\n\r\n");
		if (pos5 != string::npos)
			tmp.value = parts[i].substr(pos5 + 4);
		this->multipart_.push_back(tmp);
	}
}

void Request::chunkedDecode()
{
	size_t pos = 0;
	string tmp;

	while ((pos = body_.find("\r\n")) != string::npos)
	{
		string size = body_.substr(0, pos);
		int len = strtol(size.c_str(), NULL, 16);
		if (len == 0)
			break;
		tmp += body_.substr(pos + 2, len);
		body_ = body_.substr(pos + 2 + len + 2);
	}

	body_ = tmp;
}

Request::Request(std::string request)
{
	if (request.empty())
		return;
	request_ = request;
	isBody_ = false;
	splitLines();
	parseFirstLine();
	parseHeaders();
	parseBody();
	if (isChunked())
		chunkedDecode();
}

/*-------------------- Getters --------------------*/

const std::string &Request::getMethod() const
{
	return (method_);
}

const std::string &Request::getURL() const
{
	return ( url_);
}

const std::string &Request::getProtocol() const
{
	return (protocol_);
}

std::string &Request::getBody()
{
	return (body_);
}

bool Request::isChunked()
{
	map <string, string>::iterator it = headers_.find("Transfer-Encoding");
	
	if (it != headers_.end() && it->second == "chunked")
		return (true);
	return (false);
}

const std::map<std::string, std::string> &Request::getHeaders() const
{
	return (headers_);
}

std::string Request::getContentType()
{
	return this->headers_["Content-Type"];
}

int Request::getContentLength()
{
	std::string length = this->headers_["Content-Length"];
	return atoi(length.c_str());
}

int Request::getPort()
{
	std::string host = this->headers_["Host"];
	size_t pos = host.find(":");
	if (pos != std::string::npos)
		return atoi(host.substr(pos + 1).c_str());
	return -1;
}
std::string Request::getHostName()
{
	std::string host = this->headers_["Host"];
	size_t pos = host.find(":");
	if (pos != std::string::npos)
		return host.substr(0, pos);
	return host;
}

vector<s_tuple> &Request::getMultipart()
{
	if (!this->body_.empty() && getContentType().find("multipart/form-data") != string::npos)
		parseMultipart();
	return (multipart_);
}

/*-------------------- Tmp --------------------*/

void Request::printHeaders()
{
	std::map<std::string, std::string>::iterator it = this->headers_.begin();
	while (it != this->headers_.end())
	{
		std::cout << it->first << std::endl;
		std::cout << it->second << std::endl;
		it++;
	}
}
void Request::setBody(std::string body)
{
	this->body_ = body;
}

std::ostream &operator<<(std::ostream &stream, Request &req)
{	
	std::time_t currentTime;
	std::time(&currentTime);
	char buffer[80];
	std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&currentTime));

	stream << BLUE << "[REQUEST " << buffer << "] " << RESET << GREEN;
	if (req.getHostName().find(":") != std::string::npos)
		stream << req.getHostName() << ":" << req.getPort() << " " << req.getMethod() << " " << req.getURL() << RESET << std::endl;	
	else
		stream << req.getHostName() << " " << req.getMethod() << " " << req.getURL() << RESET << std::endl;
	// Convert the time to a string representation
	// stream << "----------------------- Request start --------------------------------------" << std::endl;
	// stream << "method        : " << req.getMethod() << std::endl;
	// stream << "url           : " << req.getURL() << std::endl;
	// stream << "content type  : " << req.getContentType() << std::endl;
	// stream << "content length: " << req.getContentLength() << std::endl;
	// stream << "port: " << req.getPort() << std::endl;
	// std::cout << "\n---- Headers ----" << std::endl;
	// const std::map<std::string, std::string>& headers = req.getHeaders();

	// for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
	// 	std::cout << it->first << ": " << it->second << std::endl;
	// }
	// stream << "body          : " << std::endl;
	// if (req.getContentType().find("multipart/form-data") != string::npos)
	// {
	// 	vector<s_tuple > data = req.getMultipart();
	// 	for (size_t i = 0; i < data.size(); i++)
	// 	{
	// 		stream << "---------------------------" << endl;
	// 		stream << "name: " << data[i].name << endl;
	// 		stream << "filename: " << data[i].fileName << endl;
	// 		stream << "value: " << data[i].value << endl;
	// 	}
	// }
	// else
	// 	stream << req.getBody() << std::endl;
	// stream << req.getContentLength() << endl;
	// stream << req.getBody().size() << endl;
	
	// stream << "----------------------- Request end --------------------------------------" << std::endl;

	return stream;
}

void printMultiForm(vector<s_tuple> &multipart)
{
	for (size_t i = 0; i < multipart.size(); i++)
	{
		cerr << "---------------------------" << endl;
		cerr << "name: " << multipart[i].name << endl;
		cerr << "filename: " << multipart[i].fileName << endl;
		cerr << "value: " << multipart[i].value << endl;
	}
}
