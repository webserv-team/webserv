/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConf.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ogorfti <ogorfti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 18:08:24 by ogorfti           #+#    #+#             */
/*   Updated: 2023/11/11 18:57:09 by ogorfti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <vector>
#include <map>

class Location
{
	public:
		std::string path;
		std::string root;
		std::string index;
};

class ServerConf
{
	public:
		int port;
		std::string hostName;
		std::string documentRoot;
		std::map <std::string, std::string> errorPages;
		std::vector<Location> locations;
};

class ConfigFile
{
	private:
		std::vector<ServerConf> servers;
	public:
		ConfigFile(const std::string& configPath);
		const std::vector<ServerConf>& getServers();
};
