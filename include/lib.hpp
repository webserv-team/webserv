/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lib.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ogorfti <ogorfti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/31 13:25:56 by ogorfti           #+#    #+#             */
/*   Updated: 2024/01/31 13:29:21 by ogorfti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIB_HPP
# define LIB_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <vector>
#include <map>

const int PORT = 8080;
const int BUFFER_SIZE = 1024;

#define GREEN "\033[1;32m"
#define RED "\033[1;31m"
#define RESET "\033[0m"

using namespace std;

#endif