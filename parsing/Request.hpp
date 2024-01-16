/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 09:36:38 by ogorfti           #+#    #+#             */
/*   Updated: 2024/01/16 16:48:43 by hoigag           ###   ########.fr       */
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
		void parseRequest();
	public:
		Request(std::string request) : request_(request)
		{
			parseRequest();
		}
		const std::string& getMethod() const;
		const std::string& getURL() const;
		const std::string& getProtocol() const;
		const std::string& getBody() const;
		const std::map<std::string, std::string>& getHeaders() const;
};