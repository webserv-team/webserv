/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConf.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ogorfti <ogorfti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 18:08:24 by ogorfti           #+#    #+#             */
/*   Updated: 2024/01/31 13:29:03 by ogorfti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../include/lib.hpp"

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
		void handleSrv(ConfigData& server, vecIt& it);
		void serverParams(ConfigData& server, vecIt& it);
		void serverErrors(ConfigData& server, vecIt& it);
		void serverLocations(ConfigData& server, vecIt& it);

	public:
		ServerConf(const std::string& configPath);
		const std::vector<ConfigData>& getServers();
};
