/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/25 10:11:55 by hoigag            #+#    #+#             */
/*   Updated: 2024/01/26 12:53:44 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include <string>
// DOCUMENT_ROOT	The root directory of your server
// HTTP_COOKIE	The visitor's cookie, if one is set
// HTTP_HOST	The hostname of the page being attempted
// HTTP_REFERER	The URL of the page that called your program
// HTTP_USER_AGENT	The browser type of the visitor
// HTTPS	"on" if the program is being called through a secure server
// PATH	The system path your server is running under
// QUERY_STRING	The query string (see GET, below)
// REMOTE_ADDR	The IP address of the visitor
// REMOTE_HOST	The hostname of the visitor (if your server has reverse-name-lookups on; otherwise this is the IP address again)
// REMOTE_PORT	The port the visitor is connected to on the web server
// REMOTE_USER	The visitor's username (for .htaccess-protected pages)
// REQUEST_METHOD	GET or POST
// REQUEST_URI	The interpreted pathname of the requested document or CGI (relative to the document root)
// SCRIPT_FILENAME	The full pathname of the current CGI
// SCRIPT_NAME	The interpreted pathname of the current CGI (relative to the document root)
// SERVER_ADMIN	The email address for your server's webmaster
// SERVER_NAME	Your server's fully qualified domain name (e.g. www.cgi101.com)
// SERVER_PORT	The port number your server is listening on
// SERVER_SOFTWARE	The server software you're using (e.g. Apache 1.3)

#include "parsing/Request.hpp"
#include <ostream>
#include <map>
#include <string>
#include "unistd.h"
#include "fcntl.h"
#include "unistd.h"
class Request;
class Cgi
{
	public:
		Cgi(Request& req);
		Cgi();
		~Cgi();
		std::string getServerPort();
		std::string getQueryString();
		std::string getRequestUri();
		std::string getRequestMethod();
		std::map<std::string, std::string> getVars();
		std::string executeScript(char **command);
		
		void setEnv();
		// Cgi(const Cgi& other);
		// Cgi& operator=(const Cgi& other);
	private:
		Request req;
		std::map<std::string, std::string> vars;
		std::string SERVER_PORT;
		std::string QUERY_STRING;
		std::string REQUEST_URI;
		std::string REQUEST_METHOD;
		char **env;
};

std::ostream& operator<<(std::ostream& stream, Cgi& cgi);

#endif
