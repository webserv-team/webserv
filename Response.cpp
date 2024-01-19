/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 15:22:39 by hoigag            #+#    #+#             */
/*   Updated: 2024/01/19 15:28:13 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(int statusCode, std::string contentType, std::string body)
{
    this->statusCode = statusCode;
    this->contentType = contentType;
    this->body = body;
    this->contentLength = this->body.length();
}

Response::~Response()
{

}

Response::Response(const Response& other)
{

}

Response& Response::operator=(const Response& other)
{

}

