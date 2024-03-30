/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/25 10:11:55 by hoigag            #+#    #+#             */
/*   Updated: 2024/03/30 22:05:00 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include <string>
#include <ostream>
#include <map>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include "Request.hpp"
#include "helpers.hpp"
#include "ServerConf.hpp"
#include "CgiParser.hpp"

class Request;
class Cgi
{
	public:
		Cgi(Request& req, Location& location);
		Cgi();
		~Cgi();
		std::string getServerPort();
		std::string getQueryString();
		std::string getRequestUri();
		std::string getRequestMethod();
		std::map<std::string, std::string> getVars();
		CgiParser executeCgiScript();
		
		void setEnv();
	private:
		Request req;
		std::string cgiPath;
		std::map<std::string, std::string> vars;
		char **env;
};

std::ostream& operator<<(std::ostream& stream, Cgi& cgi);

#endif
