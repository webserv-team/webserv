/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/24 14:56:22 by hoigag            #+#    #+#             */
/*   Updated: 2024/01/15 12:36:51 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

#include "WebServer.hpp"
int main()
{
    try
    {
        WebServer server;
        server.createSocket();
        server.bindSocket();
        server.listenForConnections();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}