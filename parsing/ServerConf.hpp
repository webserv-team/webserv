/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConf.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ogorfti <ogorfti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 18:08:24 by ogorfti           #+#    #+#             */
/*   Updated: 2024/02/29 18:16:47 by ogorfti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../include/lib.hpp"
using namespace std;

class Location
{
	public:
		string path;
		string root;
		string index;
		string autoindex;
		string bodyLimit;
		map <string, string> redirect;
		vector<string> methods;
};

class ConfigData
{
	public:
		vector <int> ports;
		string host;
		string root;
		string cgiPath;
		string uploadPath;
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

void	settingsError(const vector<string>& settings);
void	checkStatusCode(const string& code);
void	isFilePath(const string& path);
int		isNumber(const string& str);