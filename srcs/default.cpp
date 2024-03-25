/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   default.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ogorfti <ogorfti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/23 01:21:37 by ogorfti           #+#    #+#             */
/*   Updated: 2024/03/23 01:45:41 by ogorfti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

// The 'R' at the beginning of a string denotes a raw string literal in C++. 
// Raw string literals treat backslashes and other special characters as regular text, 
string defaultError(string errorCode, string errorMessage)
{
	string html = "<!DOCTYPE html>\n"
	"<html lang=\"en\">\n"
	"<head>\n"
	"    <meta charset=\"UTF-8\">\n"
	"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
	"    <title>" + errorCode + "</title>\n"
	"    <style>\n"
	"        body {\n"
	"            font-family: Arial, sans-serif;\n"
	"            margin: 0;\n"
	"            padding: 0;\n"
	"            background-color: #f4f4f4;\n"
	"            text-align: center;\n"
	"        }\n"
	"\n"
	"        .container {\n"
	"            max-width: 600px;\n"
	"            margin: 100px auto;\n"
	"            padding: 20px;\n"
	"            background-color: white;\n"
	"            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);\n"
	"            border-radius: 5px;\n"
	"        }\n"
	"\n"
	"        h1 {\n"
	"            color: #333;\n"
	"        }\n"
	"\n"
	"        p {\n"
	"            color: #666;\n"
	"        }\n"
	"    </style>\n"
	"</head>\n"
	"<body>\n"
	"\n"
	"    <div class=\"container\">\n"
	"        <h1>" + errorCode + "</h1>\n"
	"        <p>" + errorMessage + "</p>\n"
	"        <p><a href=\"/\">Go back to the homepage</a></p>\n"
	"    </div>\n"
	"\n"
	"</body>\n"
	"</html>\n";
	return html;
}