/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConf.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ogorfti <ogorfti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 18:08:24 by ogorfti           #+#    #+#             */
/*   Updated: 2024/02/12 11:43:51 by ogorfti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../include/lib.hpp"
using namespace std;

typedef vector<string>::iterator vecIt;

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
		// int port;
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
		void serverParams(ConfigData& server, const vector<string>& settings);
		void serverErrors(ConfigData& server, const string& errors);
		void serverLocations(ConfigData& server, const string& locations);

	public:
		ServerConf(const string& configPath);
		const vector<ConfigData>& getServers() const;
};
