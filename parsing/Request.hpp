/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ogorfti <ogorfti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 09:36:38 by ogorfti           #+#    #+#             */
/*   Updated: 2023/11/08 17:55:32 by ogorfti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <vector>
#include <map>

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
	public:
		Request(std::string request) : request_(request){}
		void parseRequest();
		const std::string& getMethod() const;
		const std::string& getURL() const;
		const std::string& getProtocol() const;
		const std::string& getBody() const;
		const std::map<std::string, std::string>& getHeaders() const;
};