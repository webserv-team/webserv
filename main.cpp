/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/24 14:56:22 by hoigag            #+#    #+#             */
/*   Updated: 2024/01/30 12:35:57 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "parsing/ServerConf.hpp"

#include "WebServer.hpp"
#include "helpers.hpp"

int main(int argc, char __unused **argv)
{
    if (argc != 2)
    {
        std::cout << "please provide a config file" << std::endl;
        return 1;
    }
        std::string configFile = argv[1];
    // getContentType("hassan");
    try
    {
        ServerConf conf(configFile);
        const std::vector<ConfigData> serversFiles = conf.getServers();
        const std::vector<WebServer> servers;
        WebServer server(serversFiles[0]);
            server.createSocket();
            server.bindSocket();
        try
        {
            server.listenForConnections();
        }
        catch(const std::exception& e)
        {
            std::cerr << "somwhere in my server" << '\n';
        }
        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}