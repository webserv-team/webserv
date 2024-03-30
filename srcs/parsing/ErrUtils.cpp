/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrUtils.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ogorfti <ogorfti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/12 14:00:49 by ogorfti           #+#    #+#             */
/*   Updated: 2024/03/30 17:27:59 by ogorfti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/lib.hpp"
#include "ServerConf.hpp"

/*-------------------- Error handling --------------------*/

int isNumber(const string& str)
{
	for (size_t i = 0; i < str.size(); i++)
	{
		if (!isdigit(str[i]))
			return 0;
	}
	return 1;
}

void	checkStatusCode(const string& code)
{
	long status = atol(code.c_str());

	if (code.empty() || status < 100 || status > 599)
		throw runtime_error("Error: Invalid status code");
	for (size_t i = 0; i < code.size(); i++)
	{
		if (!isdigit(code[i]))
			throw runtime_error("Error: Invalid status code");
	}
}

void	isFilePath(const string& path)
{
	if (path.empty())
		throw runtime_error("Error: Invalid file path");
	for (size_t i = 0; i < path.size(); i++)
	{
		if (!isalnum(path[i]) && path[i] != '/' && path[i] != '.'
			&& path[i] != '_' && path[i] != '-')
		{
			throw runtime_error("Error: Invalid file path");
		}
	}
}

void	checkValues(const string& value, const string& key)
{
	if (key.empty() || value.empty())
		throw runtime_error("Error: Missing server settings");
	for (size_t i = 0; i < value.size(); i++)
	{
		if (!isalnum(value[i]) && value[i] != '/'
			&& key != "errorPages" && key != "ports" && key != "host")
		{
			throw runtime_error("Error: Invalid server settings value");
		}
	}
}

void	checkBodyLimit(const string& bodyLimit)
{
	if (bodyLimit.empty() || bodyLimit.size() < 2)
		throw runtime_error("Error: Missing body limit");
	
	char unit = tolower(bodyLimit.back());
	if (unit != 'b' && unit != 'k' && unit != 'm' && unit != 'g')
		throw runtime_error("Error: Invalid body limit unit");
	
	for (size_t i = 0; i < bodyLimit.size() - 1; i++)
	{
		if (!isdigit(bodyLimit[i]))
			throw runtime_error("Error: Invalid body limit value");
	}
}

vector<string> splitBuffer(const string& buffer)
{
	string token;
	vector<string> servers;
	size_t start = 0;
	size_t end = 0;

	while ((end = buffer.find("[server]", start + 1)) != string::npos)
	{
		token = buffer.substr(start, end - start);
		servers.push_back(token);
		start = end;
	}
	servers.push_back(buffer.substr(start));
	return servers;
}

void	overideLocations(ConfigData& server, Location& loc)
{
	if (loc.bodyLimit.empty())
		loc.bodyLimit = server.bodyLimit;
	if (loc.uploadPath.empty())
		loc.uploadPath = server.uploadPath;
	if (loc.autoindex.empty())
		loc.autoindex = "off";
}

void nessaryFields(Location& loc)
{
	if (loc.path.empty())
		throw runtime_error("Error: Missing path in location");
	else if (loc.methods.empty())
		throw runtime_error("Error: Missing methods in location");
	else if (loc.root.empty() && loc.alias.empty())
		throw runtime_error("Error: Missing root or alias in location");
	else if (!loc.root.empty() && !loc.alias.empty())
		throw runtime_error("Error: Both root and alias are specified in location");
}

vector<string> splitLines(const string& buffer)
{
	vector<string> lines;
	size_t begin = 0;
	size_t end = 0;

	do
	{
		end = buffer.find('\n', begin);
		lines.push_back(buffer.substr(begin, end - begin));
		begin = end + 1;
	} while (end != string::npos);
	return lines;
}

string trim(const string& str)
{
	string chars;
	
	chars = " \t\n";
	string trimmed = str;
	trimmed.erase(0, trimmed.find_first_not_of(chars));
	trimmed.erase(trimmed.find_last_not_of(chars) + 1);
	return trimmed;
}

vector <string> split(const string& str, const string& delim)
{
	vector <string> tokens;
	size_t prev = 0, pos = 0;

	do 
	{
		pos = str.find(delim, prev);
		string token = str.substr(prev, pos - prev);

		tokens.push_back(token);
		prev = pos + delim.length();
	} while (pos != string::npos);
	return tokens;
}
