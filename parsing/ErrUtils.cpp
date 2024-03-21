/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrUtils.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ogorfti <ogorfti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/12 14:00:49 by ogorfti           #+#    #+#             */
/*   Updated: 2024/03/19 22:58:23 by ogorfti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/lib.hpp"

/*-------------------- Error handling --------------------*/

int isNumber(const string& str)
{
	for (size_t i = 0; i < str.size(); i++)
	{
		if (!isdigit(str[i]))
		{
			// cerr << RED << str[i] << " is not a number" << RESET << endl;
			return 0;
		}
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
		if (!isalnum(path[i]) && path[i] != '/' && path[i] != '.' && path[i] != '_' && path[i] != '-')
		{
			// cerr << RED << path[i] << RESET << endl;
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
		if (!isalnum(value[i]) && value[i] != '/' && key != "host" && key != "errorPages" && key != "ports")
		{
			// cerr << RED << value << RESET << endl;
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
