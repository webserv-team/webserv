/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ogorfti <ogorfti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 09:36:38 by ogorfti           #+#    #+#             */
/*   Updated: 2024/03/28 23:40:51 by ogorfti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <ctime>
#include <ostream>
#include <tuple>

#define GREEN "\033[1;32m"
#define RED "\033[1;31m"
#define BLUE "\033[1;34m"
#define RESET "\033[0m"
using namespace std;
struct s_tuple
{
    string name;
    string value;
    string fileName;
};
class Request
{
	private:
		std::string request_;
		std::vector<std::string> lines_;
		std::string method_;
		std::string url_;
		std::string protocol_;
		std::map <std::string, std::string> headers_;
		bool isBody_;
		std::string body_;
		vector<s_tuple > multipart_;
		void splitLines();
		void parseFirstLine();
		void parseHeaders();
		void parseBody();
		// void parseRequest();
		void parseMultipart();
		void chunkedDecode();
	public:
		
		Request();
		Request(std::string request);
		Request& operator=(Request& reaquest);
		~Request();
		void setBody(std::string body);
		void printHeaders();
		Request(Request& reaquest);
		int getContentLength();
		std::string getContentType();
		const std::string& getMethod() const;
		const std::string& getURL() const;
		const std::string& getProtocol() const;
		std::string& getBody();
		const std::map<std::string, std::string>& getHeaders() const;
		vector<s_tuple >& getMultipart();
		int getPort();
		std::string getHostName();
		bool isChunked();
};

std::ostream& operator<<(std::ostream& stream, Request& req);