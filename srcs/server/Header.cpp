/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Header.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/07 10:15:09 by hoigag            #+#    #+#             */
/*   Updated: 2024/03/29 14:17:44 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Header.hpp"

Header::Header()
{
    this->req = "";
      
};
Header::Header(std::string& req)
{
    this->req = req;
}
std::string Header::getMethod()
{
    size_t pos = this->req.find(" ");
    return this->req.substr(0, pos);
}

int Header::getContentLength()
{
    int contentlength = 0;
    size_t pos = this->req.find("Content-Length: ");
    if (pos != std::string::npos)
    {
        std::string length = req.substr(pos + 16);
        contentlength = atoi(length.c_str());
    }
    return contentlength;
}

bool Header::isChunked()
{
    size_t pos = this->req.find("Transfer-Encoding: chunked");
    if (pos != std::string::npos)
        return true;
    return false;
}

Header::~Header()
{

}

Header::Header(const Header& other)
{
    *this = other;
}

Header& Header::operator=(const Header& other)
{
    this->req = other.req;
    return *this;
}

