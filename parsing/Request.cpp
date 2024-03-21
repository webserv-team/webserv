/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/01 16:18:37 by ogorfti           #+#    #+#             */
/*   Updated: 2024/03/19 15:54:29 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

/*-------------------- Constructors --------------------*/

Request::Request(Request &request)
{
	*this = request;
}

Request::Request() {}

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
	size_t start = 14;
	size_t pos = 0;

	// std::cout << "body_ == " << body_.size() << std::endl;
	while ((pos = body_.find("\r\n\r\n", start)) != std::string::npos)
	{
		s_tuple tmp;
		string header = body_.substr(start, pos - start);

		size_t pos1 = header.find("name=\"");
		if (pos1 != string::npos)
		{
			size_t pos2 = header.find("\"", pos1 + 6);
			tmp.name = header.substr(pos1 + 6, pos2 - pos1 - 6);
		}
		size_t pos3 = header.find("filename=\"");
		if (pos3 != string::npos)
		{
			size_t pos4 = header.find("\"", pos3 + 10);
			tmp.fileName = header.substr(pos3 + 10, pos4 - pos3 - 10);
		}
		size_t pos5 = body_.find("\r\n", pos + 4);
		if (pos5 != string::npos)
		{
			// tmp.value = body_.substr(pos + 4, pos5 - pos - 4);
			tmp.value = body_.substr(pos + 4);
			// std::cout << "after body_ == " << tmp.value.size() << std::endl;
		}
		this->multipart_.push_back(tmp);
		start = pos + 4;
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
		tmp += body_.substr(pos + 2, len) + "\n";
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
	if (!body_.empty() && headers_["Transfer-Encoding"] == "chunked")
		chunkedDecode();
	return (body_);
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
	return "localhost";
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
#include <ctime>
std::ostream &operator<<(std::ostream &stream, Request &req)
{	
    std::time_t currentTime;
    std::time(&currentTime);
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&currentTime));

	// stream << "\033[32m[REQUEST " << buffer << "]\033[0m " << req.getHostName() << ":" << req.getPort() << " " << req.getMethod() << " " << req.getURL() << std::endl;	
    stream << BLUE << "[REQUEST " << buffer << "] " << RESET << GREEN << req.getHostName() << ":" << req.getPort() << " " << req.getMethod() << " " << req.getURL() << RESET << std::endl;
	// Convert the time to a string representation
	// stream << "----------------------- Request start --------------------------------------" << std::endl;
	// stream << "method        : " << req.getMethod() << std::endl;
	// stream << "url           : " << req.getURL() << std::endl;
	// stream << "content type  : " << req.getContentType() << std::endl;
	// stream << "content length: " << req.getContentLength() << std::endl;
	// stream << "port: " << req.getPort() << std::endl;
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

/*-------------------- Main --------------------*/

// c++ -std=c++98 -Wall -Wextra -Werror Request.cpp && ./a.out
// int main()
// {
// 	std::string httpRequest = "POST /test HTTP/1.1\r\n";
// 	httpRequest += "Host: www.test.com\r\n";
// 	httpRequest += "Transfer-Encoding: chunked\r\n";
// 	httpRequest += "Content-Type: application/json\r\n\r\n";

// 	httpRequest += "26\r\n";  // 38 in hexadecimal
// 	httpRequest += "{\"test1\": \"value1\", \"test2\": \"value2\"}\r\n";

// 	httpRequest += "3A\r\n";  // 58 in hexadecimal
// 	httpRequest += "{\"test3\": \"value3\", \"test4\": \"value4\", \"test5\": \"value5\"}\r\n";

// 	httpRequest += "0\r\n";
// 	httpRequest += "\r\n";

// // 	httpRequest += "--myboundary--\r\n";
// 	Request parser(httpRequest);
	// parser.chunkedDecode();
	// string body = parser.getBody();
	// chunkedDecode(body);
// 	vector <s_tuple> multipart = parser.getMultipart();
// 	printMultiForm(multipart);
	// parser.parseRequest();

	// std::cout << parser << std::endl;
	// std::cout << "Request:" << std::endl << httpRequest << std::endl;
	// std::cout << "--------" << std::endl;
	// std::cout << "Method: " << parser.getMethod() << std::endl;
	// std::cout << "URL: " << parser.getURL() << std::endl;
	// std::cout << "Protocol: " << parser.getProtocol() << std::endl;

	// std::cout << "--------" << std::endl;
	// std::cout << "\n---- Headers ----" << std::endl;
	// const std::map<std::string, std::string>& headers = parser.getHeaders();

	// for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
	// 	std::cout << it->first << ": " << it->second << std::endl;
	// }

// 	std::cout << "\n---- Body ----" << std::endl;
// 	std::cout << parser.getBody() << std::endl;

// 	// parseMultipart();

// 	return 0;
// }