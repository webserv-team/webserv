/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/01 16:18:37 by ogorfti           #+#    #+#             */
/*   Updated: 2024/02/09 16:06:52 by hoigag           ###   ########.fr       */
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
		// if (!lines_[i].find("{") || !lines_[i].find("--myboundary"))
		// 	isBody_ = true;
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
const std::string &Request::getBody() const
{
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
std::ostream &operator<<(std::ostream &stream, Request &req)
{
	stream << "----------------------- Request start --------------------------------------" << std::endl;
	stream << "method        : " << req.getMethod() << std::endl;
	stream << "url           : " << req.getURL() << std::endl;
	stream << "content type  : " << req.getContentType() << std::endl;
	stream << "content length: " << req.getContentLength() << std::endl;
	stream << "body          : " << std::endl;
	if (req.getContentType().find("multipart/form-data") != string::npos)
	{
		vector<s_tuple > data = req.getMultipart();
		for (size_t i = 0; i < data.size(); i++)
		{
			stream << "---------------------------" << endl;
			stream << "name: " << data[i].name << endl;
			stream << "filename: " << data[i].fileName << endl;
			stream << "value: " << data[i].value << endl;
		}
	}
	else
		stream << req.getBody() << std::endl;
	stream << "----------------------- Request end --------------------------------------" << std::endl;

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

void Request::parseMultipart()
{
	size_t start = 14;
	size_t pos = 0;

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
			tmp.value = body_.substr(pos + 4, pos5 - pos - 4);
		}
		this->multipart_.push_back(tmp);
		start = pos + 4;
	}
}

// c++ -std=c++98 -Wall -Wextra -Werror Request.cpp && ./a.out
// int main()
// {
// 	std::string httpRequest = "POST /api/data HTTP/1.1\r\n";
// 	httpRequest += "Host: api.example.com\r\n";
// 	httpRequest += "User-Agent: Mozilla/5.0\r\n";
// 	httpRequest += "Accept: */*\r\n";
// 	httpRequest += "Accept-Language: en-US,en;q=0.5\r\n";
// 	httpRequest += "Accept-Encoding: gzip, deflate, br\r\n";
// 	httpRequest += "Connection: keep-alive\r\n";
// 	httpRequest += "Content-Type: multipart/form-data; boundary=myboundary\r\n\r\n";

// 	httpRequest += "--myboundary\r\n";
// 	httpRequest += "Content-Disposition: form-data; name=\"field1\"\r\n\r\n";
// 	httpRequest += "value1\r\n";

// 	httpRequest += "--myboundary\r\n";
// 	httpRequest += "Content-Disposition: form-data; name=\"field2\"\r\n\r\n";
// 	httpRequest += "value2\r\n";

// 	httpRequest += "--myboundary\r\n";
// 	httpRequest += "Content-Disposition: form-data; name=\"image\"; filename=\"example.jpg\"\r\n";
// 	httpRequest += "Content-Type: image/jpeg\r\n\r\n";
// 	httpRequest += "(binary content of the image here)\r\n";

// 	httpRequest += "--myboundary--\r\n";
// 	Request parser(httpRequest);
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

	// std::cout << "\n---- Body ----" << std::endl;
	// std::cout << parser.getBody() << std::endl;

	// parseMultipart();

// 	return 0;
// }