/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emohamed <emohamed@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/23 11:26:17 by emohamed          #+#    #+#             */
/*   Updated: 2024/01/27 12:22:18 by emohamed         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <stdlib.h>
#include <netinet/in.h>
#include <string>
#include <string.h>
#include <sstream>
#include <fstream>
#include "../parsing/ServerConf.hpp"
#include "../parsing/Request.hpp"
struct sockaddr_in address;
#define BUFFER_SIZE 4096

std::string Response(std::string filename){
    std::fstream file;
    std::string line;
    std::string content;

    
}

int main(){
    ServerConf data("../default.toml");
    int port = data.getServers()[0].port;
    std::cout << port << std::endl;
    int new_socket;
    int server = socket(AF_INET, SOCK_STREAM, 0);
    if(server < 0){
        std::cerr << "Socket creation error" << std::endl;
        exit(1);
    }
    int opt = 1;
    if(setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1){
        std::cerr << "Setsockopt eroor: " << strerror(errno) << std::endl;
        return -1;
    }
    memset((char *)&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);
    int b = bind(server, (struct sockaddr *)&address, sizeof(address));
     if(b < 0){
        std::cerr << "Bind failed" << std::endl;
        exit(1);
    }
    int l = listen(server, 20);
    int addrlen = sizeof(address);
    
    while(true){
        std::cout << "          \033[1;32m 🛠 ...... SERVER ON ...... 🛠\033[0m" << std::endl;
        new_socket = accept(server, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if(l < 0 || new_socket < 0){
            std::cerr << "cant accept or lestining .." << std::endl;
        }
        char *buff = new char[BUFFER_SIZE];
        int r = read(new_socket, buff, BUFFER_SIZE);
        if( r < 0){
            std::cerr << "cant read from the socket" << std::endl;
        }
        buff[r] = '\0';
        std::cout << buff << std::endl;
        Request req(buff);
        req.parseRequest();
        std::string path = req.getURL();
        std::cout << path << std::endl;
    }
}