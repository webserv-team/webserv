/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpServer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 17:47:07 by hoigag            #+#    #+#             */
/*   Updated: 2024/03/27 21:58:48 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPSERVER_HPP
#define HTTPSERVER_HPP

#include <iostream>
#include "ServerConf.hpp"
#include <vector>
#include "Socket.hpp"

#define PURPLE "\033[1;35m"
#define RESET "\033[0m"

class HttpServer
{
	public:
		HttpServer();
		HttpServer(ConfigData& confData);
		~HttpServer();
		void start(fd_set& read_sockets, int& maxFd);
		ConfigData getConfData();
		std::vector<Socket> getSockets();
		HttpServer(const HttpServer& other);
		HttpServer& operator=(const HttpServer& other);
	private:
		std::vector<Socket> portFds;
		ConfigData conf;
};

#endif
