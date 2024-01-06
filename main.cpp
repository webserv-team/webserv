/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/24 14:56:22 by hoigag            #+#    #+#             */
/*   Updated: 2023/12/25 15:59:12 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>


#define SERVER_PORT 3030
#define LEN 1024
#include "WebServer.hpp"
int main()
{
    try
    {
        WebServer server;
        server.createSocket();
        server.bindSocket();
        server.listenForConnections();
        /* code */
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
//     int serverFd, connFd;
//     struct sockaddr_in servaddr;
    // char res[LEN];
    //* creating the socket


    //*setting up the server


    //     // close(connFd);
    // }
    // close(serverFd);
}