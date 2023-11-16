/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConf.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ogorfti <ogorfti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 19:01:46 by ogorfti           #+#    #+#             */
/*   Updated: 2023/11/16 11:47:34 by ogorfti          ###   ########.fr       */
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
		line_.push_back(buffer.substr(begin, end - begin));
		begin = end + 1;
	}
}

ServerConf::ServerConf(const std::string& configPath)
{
	std::string buffer = readFile(configPath);
	splitLines(buffer);
	// std::cout << buffer << '\n';

	for (std::vector<std::string>::iterator it = line_.begin(); it != line_.end(); it++)
	{
		std::cout << *it << '\n';;
	}
}

int main()
{
	ServerConf("../default.toml");
}