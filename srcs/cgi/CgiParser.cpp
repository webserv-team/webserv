/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiParser.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/28 21:59:25 by hoigag            #+#    #+#             */
/*   Updated: 2024/03/29 12:09:35 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiParser.hpp"

CgiParser::CgiParser()
{
    
}

std::string CgiParser::getValueOf(std::string key)
{
    if (this->header.empty())
		return "";
	size_t pos = this->header.find(key);
	if (pos == std::string::npos)
		return "";
	std::string contentType = this->header.substr(pos + key.length() + 2);
	pos = contentType.find("\r\n");
	if (pos != std::string::npos)
		contentType = contentType.substr(0, pos);
	return contentType;
}

CgiParser::CgiParser(std::string& content)
{
    size_t pos = content.find("\r\n\r\n");
    if (pos != std::string::npos)
    {
        this->header = content.substr(0, pos);
        this->body = content.substr(pos + 4);
    }
}

std::string CgiParser::getContentType()
{
    return this->getValueOf("Content-Type");
}

std::string CgiParser::getContentLength()
{
    return this->getValueOf("Content-Length");
}

std::string CgiParser::getLocation()
{
    return this->getValueOf("Location");
}

std::string CgiParser::getStatusCode()
{
    std::string statusLine = this->getValueOf("Status");
    std::string statusCode = statusLine.substr(0, statusLine.find(" "));
    return statusCode;
}

std::string CgiParser::getBody()
{
    return this->body;
}

CgiParser::~CgiParser()
{

}

// CgiParser::CgiParser(const CgiParser& other)
// {

// }

// CgiParser& CgiParser::operator=(const CgiParser& other)
// {

// }

