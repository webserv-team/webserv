/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConf.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ogorfti <ogorfti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 19:01:46 by ogorfti           #+#    #+#             */
/*   Updated: 2023/11/16 17:38:42 by ogorfti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConf.hpp"

const std::vector<ConfigData>& ServerConf::getServers() { return this->servers; }

std::string readFile(const std::string& configPath)
{
	std::ifstream file;

	//constructor of std::ifstream expected a C-style string c++98
	file.open(configPath.c_str());
	if (!file.is_open())
	{
		std::cerr << "Error: Unable to open the config file" << std::endl;
		exit (1);
	}
	//std::ostringstream to read the entire content of the file into a string
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

void ServerConf::parseServer(ConfigData& server, vecIt& it)
{
	std::cout << "here 0" << '\n';
	int check = 0;
	while (it != lines_.end() && *it != "[[servers]]")
	{
		if (!check)
		{
			check = 1;			
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
		}
		else if (it->find("servers.errorPages") != std::string::npos)
		{	
			std::cout << "[servers.errorPages]" << '\n';
		}
		else if (it->find("servers.locations") != std::string::npos)
		{
			std::cout << "[servers.locations]" << '\n';
		}
		it++;
	}
	it--;
}

void ServerConf::startParse()
{
	vecIt it = lines_.begin();

	for (; it != lines_.end(); it++)
	{
		if (*it == "[[servers]]")
		{
			it++;
			ConfigData server;
			parseServer(server, it);
			servers.push_back(server);
			std::cout << "port : " << server.port << '\n'; 
		}
	}
}

ServerConf::ServerConf(const std::string& configPath)
{
	std::string buffer = readFile(configPath);
	splitLines(buffer);
	startParse();
}

int main()
{
	ServerConf("../default.toml");
}