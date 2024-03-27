/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   responseHelpers.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 19:19:57 by hoigag            #+#    #+#             */
/*   Updated: 2024/03/27 22:17:57 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSEHELPERS_HPP
#define RESPONSEHELPERS_HPP

#include <string>
#include "Request.hpp"
#include "ServerConf.hpp"
#include <dirent.h>

using namespace std;

string getStatusReason(string& status);
long long toBytes(string& bodyLimit);
bool bodyLimitExceeded(Request& req, Location& loc);
bool chrURL(string& url);
bool verifyPostReq(Request& req);
bool removeDir(const string& path);
bool transferEncodingChunked(Request& req);
// bool isFileExists(const std::string& path);
Location getMatchingLocation(const string& url, ConfigData& conf);

#endif