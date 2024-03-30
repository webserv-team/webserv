/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mimes.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 12:32:37 by hoigag            #+#    #+#             */
/*   Updated: 2024/03/30 17:33:39 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MIMES_HPP
#define MIMES_HPP

#include <iostream>
#include <map>
#include <fstream>
#include <sstream>
#include "helpers.hpp"
class Mimes
{
	public:
		Mimes();
		~Mimes();
		std::string getContentType(std::string& ext);
		bool isMime(std::string& ext);
		bool isValidContentType(std::string& contentType);
	private:
		void loadFile();
		void parseLine(std::string& line);
		std::string filename;
		std::map<std::string, std::string> extensions;
};

#endif
