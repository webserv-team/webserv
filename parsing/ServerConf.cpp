/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConf.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ogorfti <ogorfti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 19:01:46 by ogorfti           #+#    #+#             */
/*   Updated: 2024/02/09 18:42:46 by ogorfti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConf.hpp"

/*-------------------- Getters --------------------*/

const vector<ConfigData>& ServerConf::getServers() { return this->servers; }

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

	while ((end = buffer.find('\n', begin)) != string::npos)
	{
		lines.push_back(buffer.substr(begin, end - begin));
		begin = end + 1;
	}
	return lines;
}

// int skip(const char* str)
// {
// 	int i = 0;
// 	while (str[i] && str[i] == 32)
// 		i++;
// 	return (i);
// }




// void ServerConf::serverLocations(ConfigData& server, vecIt& it)
// {
// 	Location tmp;

// 	while (it != lines_.end() && !it->empty())
// 	{
// 		size_t pos = it->find('=');
// 		if (pos != string::npos)
// 		{
// 			string key = it->substr(skip(it->c_str()), pos - skip(it->c_str()) - 1);
// 			string value = it->substr(pos + 2);
// 			if (key == "path")
// 				tmp.path = value;
// 			else if (key == "root")
// 				tmp.root = value;
// 			else if (key == "index")
// 				tmp.index = value;
// 		}
// 		it++;
// 	}
// 	server.locations.push_back(tmp);
// 	//! possible SGV
// 	it--;
// }

void ServerConf::serverErrors(ConfigData& server, const string& errors)
{
	cerr << errors << endl;
	(void)server;
	// while (it != lines_.end() && !it->empty())
	// {
	// 	size_t pos = it->find('=');
	// 	if (pos != string::npos)
	// 	{
	// 		string key = it->substr(skip(it->c_str()), pos - skip(it->c_str()) - 1);
	// 		string value = it->substr(pos + 2);
	// 		server.errorPages[key] = value;
	// 	}
	// 	it++;
	// }
	// it--;
}

string trim(const string& str)
{
    const string chars = " \t\n\v\f\r\"";
    string trimmed = str;
    trimmed.erase(0, trimmed.find_first_not_of(chars));
    trimmed.erase(trimmed.find_last_not_of(chars) + 1);
    return trimmed;
}

void ServerConf::serverParams(ConfigData& server, const vector<string>& settings)
{
	for (size_t i = 0; i < settings.size(); i++)
	{
		// cerr << settings[i] << endl;
		size_t equalPos = settings[i].find('=');
		if (equalPos != string::npos)
		{
			string key = trim(settings[i].substr(0, equalPos));
			string value = trim(settings[i].substr(equalPos + 1));
			// cerr << "key: *" << key << "* value: *" << value << "*"<< endl;
			if (key == "port")
				server.port = atoi(value.c_str());
			else if (key == "host")
				server.host = value;
			else if (key == "root")
				server.root = value;
			else if (key == "cgiPath")
				server.cgiPath = value;
			else if (key == "uploadPath")
				server.uploadPath = value;
			else if (key == "errorPages")
			{
				serverErrors(server, value);
				// cerr << "errorPages" << endl;
			}
		}
	}
}

void ServerConf::handleSrv(ConfigData& server, const string& lines)
{
	string settings = lines.substr(0, lines.find("\n\n") + 1);
	string locations = lines.substr(settings.size() + 2);
	// cerr << settings << endl;
	// cerr << locations << endl;
	serverParams(server, splitLines(settings));
}

vector<string> splitBuffer(const string& buffer)
{
	string token;
    vector<string> servers;
    size_t start = 0;
    size_t end = 0;
    while ((end = buffer.find("[[server]]", start + 1)) != string::npos)
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
		vector<string> vecBuffer = splitBuffer(buffer);

		for (size_t i = 0; i < vecBuffer.size(); i++)
		{
			// cerr << vecBuffer[i] << endl;
			// cerr << "-----------------------------\n";

			ConfigData server;
			handleSrv(server, vecBuffer[i]);
		}
	}
	catch (exception& e)
	{
		cerr << e.what() << endl;
	}
}

int main()
{
    ServerConf test("../default.toml");
}
