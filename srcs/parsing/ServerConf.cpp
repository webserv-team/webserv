/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConf.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ogorfti <ogorfti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 19:01:46 by ogorfti           #+#    #+#             */
/*   Updated: 2024/03/30 17:30:59 by ogorfti          ###   ########.fr       */
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
			if (key == "path" || key == "root" || key == "index"
				|| key == "uploadPath" || key == "cgiPath" || key == "alias")
				isFilePath(value);
			if (key == "path")
				loc.path = value;
			else if (key == "root")
				loc.root = value;
			else if (key == "index")
				loc.index = value;
			else if (key == "uploadPath")
				loc.uploadPath = value;
			else if (key == "cgiPath")
				loc.cgiPath = value;
			else if (key == "alias")
				loc.alias = value;
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
	overideLocations(server, loc);
	nessaryFields(loc);
	return loc;
}

void	serverLocations(ConfigData& server, const string& locations)
{
	vector<string> tokens = split(locations.substr(18), "[server.location]");

	server.hasDefault = false;
	for (size_t i = 0; i < tokens.size(); i++)
	{
		Location loc = parseLocation(tokens[i], server);
		if (loc.path == "/")
			server.hasDefault = true;
		server.locations.push_back(loc);
	}
	if (!server.hasDefault)
		throw runtime_error("Error: Missing default location");
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
		if (!isNumber(trim(tmp[i])) || tmp[i].empty() || port < 0 || port > 65535
			|| find(server.ports.begin(), server.ports.end(), port) != server.ports.end())
			throw runtime_error("Error: Invalid port number");
		server.ports.push_back(atoi(tmp[i].c_str()));
	}
}

void	serverParams(ConfigData& server, const vector<string>& settings)
{
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
			else if (key == "serverName")
				server.serverName = value;
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
			throw runtime_error("Error: Invalid server settings configuration");
	}
}

ServerConf::ServerConf(const string& configPath)
{
	string buffer = readFile(configPath);
	vector<string> vecBuffer = splitBuffer(trim(buffer));

	for (size_t i = 0; i < vecBuffer.size(); i++)
	{
		ConfigData server;

		string settings = vecBuffer[i].substr(0, vecBuffer[i].find("[server.location]"));
		string locations = vecBuffer[i].substr(settings.size());
		
		settings = trim(settings);
		serverParams(server, splitLines(settings));
		serverLocations(server, locations);
		servers.push_back(server);
	}
}

/*-------------------- Testing --------------------*/

ostream& operator<<(ostream& os, const Location& location)
{
	os << "path: " << location.path << endl;
	os << "root: " << location.root << endl;
	os << "index: " << location.index << endl;
	os << "autoindex: " << location.autoindex << endl;
	os << "bodyLimit: " << location.bodyLimit << endl;
	os << "uploadPath: " << location.uploadPath << endl;
	os << "cgiPath: " << location.cgiPath << endl;
	os << "alias: " << location.alias << endl;
	os << "redirect: " << endl;
	for (map<string, string>::const_iterator it = location.redirect.begin(); it != location.redirect.end(); it++)
		os << it->first << "-   -" << it->second << endl;
	os << "methods: ";
	for (size_t i = 0; i < location.methods.size(); i++)
		os << location.methods[i] << " ";
	return os;
}

ostream& operator<<(ostream& os, const vector<ConfigData>& servers)
{
	for (size_t i = 0; i < servers.size(); i++)
	{
		os << "----------- Ports ------------\n";
		for (size_t j = 0; j < servers[i].ports.size(); j++)
		{
			os << servers[i].ports[j] << endl;
		}
		os << "--------- Settings -----------\n";
		os << "host: " << servers[i].host << endl;
		os << "root: " << servers[i].root << endl;
		os << "bodyLimit: " << servers[i].bodyLimit << endl;
		os << "serverName: " << servers[i].serverName << endl;
		os << "uploadPath: " << servers[i].uploadPath << endl;
		os << "errorPages: " << endl;
		for (map<string, string>::const_iterator it = servers[i].errorPages.begin(); it != servers[i].errorPages.end(); it++)
			os << it->first << "-   -" << it->second << endl;
		os << "\n--------- Locations -----------" << servers[i].locations.size() << "\n";
		for (size_t j = 0; j < servers[i].locations.size(); j++)
		{
			os << servers[i].locations[j] << endl;
			os << "----------------------\n";
		}
	}
	return os;
}

// int main()
// {
// 	try
// 	{
// 		ServerConf test("../config/ogorfti.conf");
		
// 		vector<ConfigData> servers = test.getServers();
// 		cout << servers;
// 	}
// 	catch (exception& e)
// 	{
// 		cerr << e.what() << endl;
// 	}
// 	return 0;
// }
