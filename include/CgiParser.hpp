/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiParser.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/28 21:59:20 by hoigag            #+#    #+#             */
/*   Updated: 2024/03/29 12:08:06 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIPARSER_HPP
#define CGIPARSER_HPP

#include <string>

class CgiParser
{
	public:
		CgiParser();
		CgiParser(std::string& content);
		~CgiParser();
		// CgiParser(const CgiParser& other);
		// CgiParser& operator=(const CgiParser& other);
		std::string getValueOf(std::string key);
		
		std::string getContentType();
		std::string getContentLength();
		std::string getLocation();
		std::string getBody();
		std::string getStatusCode();
	private:
		std::string body;
		std::string header;
};

#endif
