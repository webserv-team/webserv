/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/27 10:37:00 by hoigag            #+#    #+#             */
/*   Updated: 2024/03/09 17:22:41 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <string>

#include "fcntl.h"
#include "unistd.h"
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <sys/stat.h>
#include <sys/socket.h>
#include <dirent.h>
#include "Request.hpp"
#include "Header.hpp"

#define BUFFER_SIZE 100000

void setSocketToBeReusable(int sock);
void setSocketToNonBlocking(int socket);
std::string readfromFd(int fd);
std::string getFileExtension(std::string filename);
bool isSupportedCgiScript(std::string script);
std::string loadFile(const std::string& path);
std::string getContentType(std::string file);
std::string getContentTypeFromCgiOutput(std::string& content);
bool isDirectory(std::string& path);
std::string directoryListing(std::string& path);
void uploadFiles(Request& req);
std::string sread(int socket);
// int sendChunk(int sock, ClientResponse& cr);

#endif