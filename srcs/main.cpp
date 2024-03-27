/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/24 14:56:22 by hoigag            #+#    #+#             */
/*   Updated: 2024/03/27 18:09:29 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "ServerConf.hpp"
#include <map>
#include "Socket.hpp"
#include "ServersMonitor.hpp"
#include "helpers.hpp"
#include  <signal.h>
#include "HttpServer.hpp"
int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cout << "please provide a config file" << std::endl;
        return 1;
    }
    std::string configFile = argv[1];
    ServerConf conf(configFile);
    std::vector<ConfigData> serversConf = conf.getServers();

    std::vector<HttpServer> httpServers;
    for (size_t i = 0; i < serversConf.size(); i++)
        httpServers.push_back(HttpServer(serversConf[i]));
    try
    {
        signal(SIGPIPE, SIG_IGN);
        ServersMonitor entry(httpServers);
        entry.listenForConnections();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}