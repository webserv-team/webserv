/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Header.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/07 10:15:02 by hoigag            #+#    #+#             */
/*   Updated: 2024/03/07 10:48:07 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEADER_HPP
#define HEADER_HPP

#include <string>

class Header
{
	public:
		Header();
		Header(std::string& req);
		~Header();
		Header(const Header& other);
		Header& operator=(const Header& other);

        std::string getMethod();
        int getContentLength();
        bool isChunked();

	private:
		std::string req;
};

#endif
