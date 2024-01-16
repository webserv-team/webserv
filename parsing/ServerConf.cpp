/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConf.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ogorfti <ogorfti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 19:01:46 by ogorfti           #+#    #+#             */
/*   Updated: 2023/11/17 11:44:01 by ogorfti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConf.hpp"

const std::vector<ConfigData>& ServerConf::getServers() { return this->servers; }

std::string readFile(const std::string& configPath)
{
	std::ifstream file;

	file.open(configPath.c_str());
	if (!file.is_open())
	{
		std::cerr << "Error: Unable to open the config file" << std::endl;
		exit (1);
	}
	std::ostringstream buffer;
	buffer << file.rdbuf();
	file.close();
	return buffer.str();
}

void ServerConf::splitLines(const std::string& buffer)
{
	size_t begin = 0;
	size_t end = 0;

	while ((end = buffer.find('\n', begin)) != std::string::npos)
	{
		lines_.push_back(buffer.substr(begin, end - begin));
		begin = end + 1;
	}
}

int skip(const char* str)
{
	int i = 0;
	while (str[i] && str[i] == 32)
		i++;
	return (i);
}

void ServerConf::serverParams(ConfigData& server, vecIt& it)
{
	while (it != lines_.end() && !it->empty())
	{
		size_t equalPos = it->find('=');
		if (equalPos != std::string::npos)
		{
			std::string key = it->substr(skip(it->c_str()), equalPos - skip(it->c_str()) - 1);
			std::string value = it->substr(equalPos + 2);
			if (key == "port")
				server.port = std::atoi(value.c_str());
			else if (key == "hostName")
				server.hostName = value;
			else if (key == "documentRoot")
				server.documentRoot = value;
		}
		it++;
	}
	//! possible SGV
	it--;
}

void ServerConf::serverErrors(ConfigData& server, vecIt& it)
{
	while (it != lines_.end() && !it->empty())
	{
		size_t pos = it->find('=');
		if (pos != std::string::npos)
		{
			std::string key = it->substr(skip(it->c_str()), pos - skip(it->c_str()) - 1);
			std::string value = it->substr(pos + 2);
			server.errorPages[key] = value;
		}
		it++;
	}
	//! possible SGV
	it--;
}

void ServerConf::serverLocations(ConfigData& server, vecIt& it)
{
	Location tmp;

	while (it != lines_.end() && !it->empty())
	{
		size_t pos = it->find('=');
		if (pos != std::string::npos)
		{
			std::string key = it->substr(skip(it->c_str()), pos - skip(it->c_str()) - 1);
			std::string value = it->substr(pos + 2);
			if (key == "path")
				tmp.path = value;
			else if (key == "root")
				tmp.root = value;
			else if (key == "index")
				tmp.index = value;
		}
		it++;
	}
	server.locations.push_back(tmp);
	//! possible SGV
	it--;
}

void ServerConf::handleSrv(ConfigData& server, vecIt& it)
{
	serverParams(server, it);
	while (it != lines_.end() && *it != "[[servers]]")
	{
		if (it->find("servers.errorPages") != std::string::npos)
		{
			serverErrors(server, it);
		}
		else if (it->find("servers.locations") != std::string::npos)
		{
			serverLocations(server, it);
		}
		it++;
	}
	//! possible SGV
	it--;
}

ServerConf::ServerConf(const std::string& configPath)
{
	std::string buffer = readFile(configPath);
	splitLines(buffer);

	for (vecIt it = lines_.begin(); it != lines_.end(); it++)
	{
		if (*it == "[[servers]]")
		{
			it++;
			ConfigData server;
			handleSrv(server, it);
			servers.push_back(server);
		}
	}
}

// int main()
// {
//     ServerConf test("../default.toml");

//     std::vector<ConfigData> servers = test.getServers();

//     for (std::vector<ConfigData>::iterator it = servers.begin(); it != servers.end(); ++it) {
//         std::cout << "Server Configuration:\n";
//         std::cout << "  Port: " << it->port << '\n';
//         std::cout << "  Hostname: " << it->hostName << '\n';
//         std::cout << "  Document Root: " << it->documentRoot << '\n';

//         std::cout << "  Error Pages:\n";
//         for (std::map<std::string, std::string>::iterator errorIt = it->errorPages.begin(); errorIt != it->errorPages.end(); ++errorIt) {
//             std::cout << "    Error Code: " << errorIt->first << ", Path: " << errorIt->second << '\n';
//         }
        
// 		std::cout << "  Locations:\n";
//         for (std::vector<Location>::iterator locIt = it->locations.begin(); locIt != it->locations.end(); ++locIt) {
//             std::cout << "    Path: " << locIt->path << ", Root: " << locIt->root << ", Index: " << locIt->index << '\n';
//         }
//         std::cout << "-----------------------------\n";
//     }
// }
