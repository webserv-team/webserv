/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 09:36:38 by ogorfti           #+#    #+#             */
/*   Updated: 2024/01/27 12:46:42 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <vector>
#include <map>
#include <ostream>

class Request
{
	private:
		std::string request_;
		std::vector<std::string> lines_;
		std::string method_;
		std::string url_;
		std::string protocol_;
		std::map <std::string, std::string> headers_;
		std::string body_;
		void splitLines();
		void parseFirstLine();
		void parseHeaders();
		void parseBody();
		void parseRequest();
	public:
		Request();
		Request(std::string request) : request_(request)
		{
			parseRequest();
		}
		void printHeaders();
		Request(Request& reaquest);
		Request& operator=(Request& reaquest);
		int getContentLength();
		std::string getContentType();
		const std::string& getMethod() const;
		const std::string& getURL() const;
		const std::string& getProtocol() const;
		const std::string& getBody() const;
		const std::map<std::string, std::string>& getHeaders() const;
};

std::ostream& operator<<(std::ostream& stream, Request& req);