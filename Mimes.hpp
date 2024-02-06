/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mimes.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 12:32:37 by hoigag            #+#    #+#             */
/*   Updated: 2024/02/04 12:48:20 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MIMES_HPP
#define MIMES_HPP

#include <iostream>
#include <map>
#include <fstream>
#include <sstream>

class Mimes
{
	public:
		Mimes();
		~Mimes();
		std::string getContentType(std::string& ext);
		// Mimes(const Mimes& other);
		// Mimes& operator=(const Mimes& other);
	private:
		void loadFile();
		void parseLine(std::string& line);
		std::string filename;
		std::map<std::string, std::string> extensions;
};

#endif
