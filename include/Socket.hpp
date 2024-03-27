/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 12:14:45 by hoigag            #+#    #+#             */
/*   Updated: 2024/03/27 18:03:13 by hoigag           ###   ########.fr       */
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
		Socket(short port, std::string& hostname);
		int getFd();
		static int acceptNewConnetction(int serverFd);
		~Socket();
		// Socket(const Socket& other);
		// Socket& operator=(const Socket& other);
	private:
		int fd;
};

#endif
