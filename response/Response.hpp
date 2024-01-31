/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ogorfti <ogorfti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/31 12:44:37 by ogorfti           #+#    #+#             */
/*   Updated: 2024/01/31 15:55:31 by ogorfti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include    "../include/lib.hpp"
#include    "../parsing/Request.hpp"
#include    "../parsing/ServerConf.hpp"

class Response
{
    public:
        static void sendResponse(int socket, Request& request, ConfigData& server);
};

void defaultError(int socket, int statusCode);
void error500(int socket, string& filePath);