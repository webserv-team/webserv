/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 13:50:27 by hoigag            #+#    #+#             */
/*   Updated: 2024/01/19 15:26:08 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>

class Response
{
	public:
		Response();
		Response(int statusCode, std::string contentType, std::string body);
		~Response();
		Response(const Response& other);
		Response& operator=(const Response& other);
	private:
		short statusCode;
		std::string contentType;
		int contentLength;
		std::string body;
		std::string statusLine;
		std::string statusReason;
};

#endif
