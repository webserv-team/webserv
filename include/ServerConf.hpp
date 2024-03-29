/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConf.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 18:08:24 by ogorfti           #+#    #+#             */
/*   Updated: 2024/03/26 16:09:42 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../include/lib.hpp"
using namespace std;
#include <algorithm>
#include <ostream>
class Location
{
	public:
		string path;
		string root;
		string index;
		string autoindex;
		string bodyLimit;
		string uploadPath;
		string cgiPath;
		map <string, string> redirect;
		vector<string> methods;
		//copy constructor
		// Location();
		// Location(const Location& location);
		// //copy assignment operator
		// Location& operator=(const Location& location);
};

class ConfigData
{
	public:
		vector <int> ports;
		string host;
		string root;
		string bodyLimit;
		string uploadPath;
		string serverName;
		map <string, string> errorPages;
		vector<Location> locations;
};

class ServerConf
{
	private:
		vector<ConfigData> servers;
	public:
		ServerConf(const string& configPath);
		const vector<ConfigData>& getServers() const;
};

void	checkStatusCode(const string& code);
void	isFilePath(const string& path);
int		isNumber(const string& str);
void	checkValues(const string& value, const string& key);
void	checkBodyLimit(const string& bodyLimit);

ostream& operator<<(ostream& os, const Location& location);