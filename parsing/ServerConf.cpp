/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConf.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 19:01:46 by ogorfti           #+#    #+#             */
/*   Updated: 2024/03/07 18:32:03 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConf.hpp"

/*-------------------- Getters --------------------*/

const vector<ConfigData>& ServerConf::getServers() const 
{
	return this->servers;
}

/*-------------------- Member functions --------------------*/

string readFile(const string& configPath)
{
	ifstream file;

	file.open(configPath.c_str());
	if (!file.is_open())
		throw runtime_error("Error: Unable to open the config file");
	ostringstream buffer;
	buffer << file.rdbuf();
	file.close();
	return buffer.str();
}

vector<string> splitLines(const string& buffer)
{
	vector<string> lines;
	size_t begin = 0;
	size_t end = 0;

	do
	{
		end = buffer.find('\n', begin);
		lines.push_back(buffer.substr(begin, end - begin));
		begin = end + 1;
	} while (end != string::npos);
	return lines;
}

string trim(const string& str)
{
	string chars;
	
	chars = " \t\n";
	string trimmed = str;
	trimmed.erase(0, trimmed.find_first_not_of(chars));
	trimmed.erase(trimmed.find_last_not_of(chars) + 1);
	return trimmed;
}

vector <string> split(const string& str, const string& delim)
{
	vector <string> tokens;
	size_t prev = 0, pos = 0;

	do 
	{
		pos = str.find(delim, prev);
		string token = str.substr(prev, pos - prev);

		// if (!token.empty() && trim(token).size() > 0)
			tokens.push_back(token);
		prev = pos + delim.length();
	} while (pos != string::npos);
	return tokens;
}

vector<string> parseMethod(const string& methods)
{
	vector<string> tokens = split(methods, ",");
	vector<string> result;

	for (size_t i = 0; i < tokens.size(); i++)
	{
		string method = trim(tokens[i]);
		if (method != "GET" && method != "POST" && method != "DELETE")
			throw runtime_error("Error: Invalid method");
		result.push_back(method);
	}
	return result;
}

Location parseLocation(const string& location, ConfigData& server)
{
	vector<string> tokens = split(location, "\n");
	Location loc;
	
	for (size_t i = 0; i < tokens.size(); i++)
	{
		size_t equalPos = tokens[i].find('=');
		if (equalPos != string::npos)
		{
			string key = trim(tokens[i].substr(0, equalPos));
			string value = trim(tokens[i].substr(equalPos + 1));
			if (key == "path" || key == "root" || key == "index")
				isFilePath(value);
			if (key == "path")				
				loc.path = value;
			else if (key == "root")
				loc.root = value;
			else if (key == "index")
				loc.index = value;
			else if (key == "autoindex")
			{
				if (value != "on" && value != "off")
					throw runtime_error("Error: Invalid autoindex value");	
				loc.autoindex = value;
			}
			else if (key == "bodyLimit")
			{
				checkBodyLimit(value);	
				loc.bodyLimit = value;
			}
			else if (key == "methods")
				loc.methods = parseMethod(value);
			else if (key == "redirect")
			{
				vector<string> redir = split(value, ",");
				if (redir.size() % 2 != 0)
					throw runtime_error("Error: Invalid redirect configuration");
				for (size_t i = 0; i < redir.size(); i += 2)
				{
					string key = trim(redir[i]);
					string value = trim(redir[i + 1]);
					isFilePath(key);
					isFilePath(value);
					loc.redirect[key] = value;
				}
			}
			else
				throw runtime_error("Error: Invalid location settings key");
		}
	}
	if (loc.bodyLimit.empty())
		loc.bodyLimit = server.bodyLimit;
	return loc;
}

void	serverLocations(ConfigData& server, const string& locations)
{
	vector<string> tokens = split(locations.substr(18), "[server.location]");

	for (size_t i = 0; i < tokens.size(); i++)
	{
		Location loc = parseLocation(tokens[i], server);
		server.locations.push_back(loc);
	}
}

void	serverErrors(ConfigData& server, const string& errors)
{
	istringstream obj(errors);
	string key, value;

	if (errors.empty() || count(errors.begin(), errors.end(), ',') % 2 == 0)
		throw runtime_error("Error: Invalid error pages configuration");
	while (getline(obj, key, ',') && getline(obj, value, ','))
	{
		key = trim(key);
		value = trim(value);

		// cerr << RED << key << " - " << value << RESET << endl;
		isFilePath(value);
		checkStatusCode(key);
		server.errorPages[key] = value;
	}
}

void	serverPorts(ConfigData& server, const string& ports)
{
	vector <string> tmp = split(ports, ",");

	for (size_t i = 0; i < tmp.size(); i++)
	{
		long port = atol(tmp[i].c_str());
		if (!isNumber(trim(tmp[i])) || tmp[i].empty() || port < 0 || port > 65535)
			throw runtime_error("Error: Invalid port number");
		server.ports.push_back(atoi(tmp[i].c_str()));
	}
}

void	serverParams(ConfigData& server, const vector<string>& settings)
{
	// settingsError(settings);
	size_t equalPos = 0;
	for (size_t i = 0; i < settings.size(); i++)
	{
		if (i == 0)
		{
			if (trim(settings[i]) != "[server]")
				throw runtime_error("Error: each server must start with [server] tag");
		}
		else if ((equalPos = settings[i].find('=')) != string::npos)
		{
			string key = trim(settings[i].substr(0, equalPos));
			string value = trim(settings[i].substr(equalPos + 1));
			checkValues(value, key);
			if (key == "ports")
				serverPorts(server, value);
			else if (key == "host")
				server.host = value;
			else if (key == "root")
				server.root = value;
			else if (key == "cgiPath")
				server.cgiPath = value;
			else if (key == "uploadPath")
				server.uploadPath = value;
			else if (key == "errorPages")
				serverErrors(server, value);
			else if (key == "bodyLimit")
			{
				checkBodyLimit(value);
				server.bodyLimit = value;
			}
			else
				throw runtime_error("Error: Invalid server settings key");
		}
		else
		{
			// cerr<<  << settings[i] << endl;
			throw runtime_error("Error: Invalid server settings configuration");
		}
	}
}

vector<string> splitBuffer(const string& buffer)
{
	string token;
	vector<string> servers;
	size_t start = 0;
	size_t end = 0;

	while ((end = buffer.find("[server]", start + 1)) != string::npos)
	{
		token = buffer.substr(start, end - start);
		servers.push_back(token);
		start = end;
	}
	servers.push_back(buffer.substr(start));
	return servers;
}

ServerConf::ServerConf(const string& configPath)
{
	try
	{
		string buffer = readFile(configPath);
		vector<string> vecBuffer = splitBuffer(trim(buffer));

		for (size_t i = 0; i < vecBuffer.size(); i++)
		{
			ConfigData server;

			string settings = vecBuffer[i].substr(0, vecBuffer[i].find("[server.location]"));
			string locations = vecBuffer[i].substr(settings.size());
			
			settings = trim(settings);
			// cerr << "<-- settings -->\n" << settings << endl;
			serverParams(server, splitLines(settings));
			serverLocations(server, locations);
			servers.push_back(server);
		}
	}
	catch (exception& e)
	{
		cerr << e.what() << endl;
	}
}

// int main()
// {
// 	ServerConf test("../default.conf");
	
// 	vector<ConfigData> servers = test.getServers();

// 	cerr << "Servers: " << servers.size() << endl;
// 	for (size_t i = 0; i < servers.size(); i++)
// 	{
// 		cerr << "----------- Ports ------------\n";
// 		for (size_t j = 0; j < servers[i].ports.size(); j++)
// 		{
// 			cerr << servers[i].ports[j] << endl;
// 		}
// 		cerr << "--------- Settings -----------\n";
// 		cerr << "host: " << servers[i].host << endl;
// 		cerr << "root: " << servers[i].root << endl;
// 		cerr << "cgiPath: " << servers[i].cgiPath << endl;
// 		cerr << "bodyLimit: " << servers[i].bodyLimit << endl;
// 		cerr << "uploadPath: " << servers[i].uploadPath << endl;
// 		cerr << "errorPages: " << endl;
// 		for (map<string, string>::iterator it = servers[i].errorPages.begin(); it != servers[i].errorPages.end(); it++)
// 			cerr << it->first << "-   -" << it->second << endl;
// 		cerr << "--------- Locations -----------" << servers[i].locations.size() << "\n";
// 		for (size_t j = 0; j < servers[i].locations.size(); j++)
// 		{
// 			cerr << "path: " << servers[i].locations[j].path << endl;
// 			cerr << "root: " << servers[i].locations[j].root << endl;
// 			cerr << "index: " << servers[i].locations[j].index << endl;
// 			cerr << "autoindex: " << servers[i].locations[j].autoindex << endl;
// 			cerr << "bodyLimit: " << servers[i].locations[j].bodyLimit << endl;
// 			cerr << "methods: ";
// 			for (size_t k = 0; k < servers[i].locations[j].methods.size(); k++)
// 				cerr << servers[i].locations[j].methods[k] << "- -";
// 			cerr << endl;
// 			cerr << "redirect: ";
// 			for (map<string, string>::iterator it = servers[i].locations[j].redirect.begin(); it != servers[i].locations[j].redirect.end(); it++)
// 				cerr << it->first << "-   -" << it->second << endl;
// 			cerr << "\n----------------------\n";
// 		}
// 	}
// }
