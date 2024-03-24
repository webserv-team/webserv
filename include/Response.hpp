/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ogorfti <ogorfti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 13:50:27 by hoigag            #+#    #+#             */
/*   Updated: 2024/03/24 22:48:08 by ogorfti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <ostream>
#include "Request.hpp"
#include "ServerConf.hpp"
#include "Cgi.hpp"
#include "helpers.hpp"
#include "Mimes.hpp"


#define GREEN "\033[1;32m"
#define RED "\033[1;31m"
#define BLUE "\033[1;34m"
#define RESET "\033[0m"

typedef struct s_data
{
	string statusCode;
	string body;
	string statusLine;
	string httpVersion;
	string statusReason;
	map<string, string> headers;
	bool isRedirect;
} t_data;

class Response
{
	private:
		Response();
		Response(const Response& other);
		void buildResponse(t_data& data);
		
		Mimes mimes;
		string response;
		t_data data;
	public:
		Response(Request &req, ConfigData &conf);
		~Response();

		std::string getResponseString();
};

// std::ostream& operator<<(std::ostream& stream, Response& res);
string defaultError(string errorCode, string errorMessage);

#endif
