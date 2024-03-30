/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConf.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ogorfti <ogorfti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 18:08:24 by ogorfti           #+#    #+#             */
/*   Updated: 2024/03/30 17:28:36 by ogorfti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/lib.hpp"

#pragma once

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
		string alias;
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
		bool hasDefault;
};

class ServerConf
{
	private:
		vector<ConfigData> servers;
		ServerConf();
		ServerConf(const ServerConf& other);
		ServerConf& operator=(const ServerConf& other);
	public:
		ServerConf(const string& configPath);
		const vector<ConfigData>& getServers() const;
};

void	checkStatusCode(const string& code);
void	isFilePath(const string& path);
int		isNumber(const string& str);
void	checkValues(const string& value, const string& key);
void	checkBodyLimit(const string& bodyLimit);
void	overideLocations(ConfigData& server, Location& loc);
void	nessaryFields(Location& loc);
string	trim(const string& str);
vector<string> splitLines(const string& buffer);
vector<string> splitBuffer(const string& buffer);
vector<string> split(const string& str, const string& delim);
ostream& operator<<(ostream& os, const Location& location);
ostream& operator<<(ostream& os, const vector<ConfigData>& servers);