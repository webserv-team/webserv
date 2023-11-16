/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConf.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ogorfti <ogorfti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 18:08:24 by ogorfti           #+#    #+#             */
/*   Updated: 2023/11/16 17:27:20 by ogorfti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

typedef std::vector<std::string>::iterator vecIt;

class Location
{
	public:
		std::string path;
		std::string root;
		std::string index;
};


class ConfigData
{
	public:
		int port;
		std::string hostName;
		std::string documentRoot;
		std::map <std::string, std::string> errorPages;
		std::vector<Location> locations;
};

class ServerConf
{
	private:
		std::vector<ConfigData> servers;
		std::vector <std::string> lines_;
		void splitLines(const std::string& buffer);
		void startParse();
		void parseServer(ConfigData& server, vecIt& it);
	public:
		ServerConf(const std::string& configPath);
		const std::vector<ConfigData>& getServers();
};
