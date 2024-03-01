/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrUtils.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ogorfti <ogorfti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/12 14:00:49 by ogorfti           #+#    #+#             */
/*   Updated: 2024/02/29 18:17:16 by ogorfti          ###   ########.fr       */
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
			cerr << RED << str[i] << " is not a number" << RESET << endl;	
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
		if (!isalnum(path[i]) && path[i] != '/' && path[i] != '.' && path[i] != '_')
			throw runtime_error("Error: Invalid file path");
	}
}

// void settingsError(const vector<string>& settings)
// {
//     if (settings.size() != 7)
//         throw runtime_error("Error: missing or extra settings");
// }