/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hassan <hassan@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 12:14:45 by hoigag            #+#    #+#             */
/*   Updated: 2024/03/08 17:36:06 by hassan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
#define SOCKET_HPP
#include "ServerConf.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <string.h>
#include "helpers.hpp"

class Socket
{
	public:
		Socket();
		Socket(const ConfigData& conf);
		int getFd();
		ConfigData getConfData();
		int start();
		static int acceptNewConnetction(int serverFd);
		~Socket();
		// Socket(const Socket& other);
		// Socket& operator=(const Socket& other);
	private:
		ConfigData conf;
		int fd;
};

#endif
