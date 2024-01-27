/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   web.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emohamed <emohamed@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/23 11:26:17 by emohamed          #+#    #+#             */
/*   Updated: 2024/01/25 12:43:43 by emohamed         ###   ########.fr       */
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

#define port 8080
struct sockaddr_in address;


std::string GetFileExtension(std::string filename) {
    size_t dotIndex = filename.find_last_of(".");
    if (dotIndex != std::string::npos) {
        return filename.substr(dotIndex + 1);
    }
    return "";
}

std::string Response( std::string filename) {
    std::fstream file;
    std::string line;
    std::string content;
    
    file.open(filename);
    std::cout << "-----------------" << std::endl;
    std::cout << "--> : "  << filename << std::endl;
    
    if (!file.is_open()) {

        std::ifstream error_file( "assets/404.html");
        std::string error_content;
        while (getline(error_file, line)) {
            error_content += line;
        }
        std::string file_size = std::to_string(error_content.size());
        std::string header = "HTTP/1.1 404 Not Found\nContent-Type: text/html\nContent-Length: " + file_size + "\n\n";
        std::string response = header + error_content;
        return response;
    }

    // while (getline(file, line)) {
    //     content += line;
    // }

    std::stringstream tmp;
    tmp << file.rdbuf();
    content = tmp.str();
    file.close();
    
    std::string file_size = std::to_string(content.size());

    std::string file_extension = GetFileExtension(filename);
    std::string content_type;
    
        if (file_extension == "html") {
        content_type = "text/html";
    } else if (file_extension == "jpg") {
        content_type = "image/jpeg";
    } else if (file_extension == "mp4") {
        content_type = "video/mp4";
    } else {
        content_type = "text/plain";
    }

    std::string header = "HTTP/1.1 200 OK\nContent-Type: " + content_type + "\nContent-Length: " + file_size + "\n\n";
    std::string response = header + content;
    
    return response;
}

int main(){
    int new_socket;
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0){
        std::cerr << "Socket creation error" << std::endl;
        exit(1);
    }
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        std::cerr << "Setsockopt error: " << strerror(errno) << std::endl;
        return (-1);
    }
    memset((char *)&address, 0, sizeof(address)); 
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);
    int b = bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    if(b < 0){
        std::cerr << "Bind failed" << std::endl;
        exit(1);
    }
    int l = listen(server_fd, 20);
    int addrlen = sizeof(address);

        while(1){
            std::cout << "          \033[1;32mListening... 🛠\033[0m" << std::endl;
            new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
            if (l < 0 || new_socket < 0){
                std::cerr << "cant accept or lestening" << std::endl;  
            }
            char *buffer = new char[1024];
            int r = read(new_socket, buffer, 1024);

            if (r < 0) {
                std::cerr << "can't read" << std::endl;
            }
            buffer[r] = '\0';
            // std::cout << buffer << std::endl;

            std::istringstream request(buffer);
            std::string method, path, protocol;
            request >> method >> path >> protocol;
            std::cout << request.str() << std::endl;

            // path.erase(0, 1);
            // std::cout << path << std::endl;

            if(path == "/"){
                path = path + "index.html";
            }
            // std::cout << "Requested file: " << path << std::endl;
            std::string response = Response("assets" + path);
            // const char *res = response.c_str();
            if (send(new_socket, response.c_str(), response.length(), 0) < 0)
            {
                std::cerr << "\n\nERROR**\n\n"; 
            }
            // write(new_socket, res, strlen(res));

            close(new_socket);
        }
    }
