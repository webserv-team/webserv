/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mimes.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 12:35:14 by hoigag            #+#    #+#             */
/*   Updated: 2024/03/25 18:28:00 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Mimes.hpp"

Mimes::Mimes()
{
    this->filename = "mimes.txt";
    this->loadFile();
}

void Mimes::parseLine(std::string& line)
{
    std::stringstream stream;
    stream.str(line);
    std::string extension;
    std::string contentType;
    std::getline(stream, extension, '|');
    std::getline(stream, contentType);
    size_t pos = extension.find(",");
    if (pos != std::string::npos)
    {
        std::stringstream stream2;
        stream2.str(extension);
        std::string ext;
        while (std::getline(stream2, ext, ','))
        {
            size_t pointPos = ext.find(".");
            if (pointPos != std::string::npos)
                ext = ext.substr(pointPos);
            this->extensions[ext] = contentType;
        }
    }
    else
    {
        this->extensions[extension] = contentType;
    }
    // std::cout << extension << std::endl;
}

void Mimes::loadFile()
{
    std::map<std::string, std::string> extensions;
    std::ifstream inFile(this->filename);
    if (!inFile.is_open())
        throw std::runtime_error("could not open mimes file");
    while (inFile)
    {
        std::string line;
        std::getline(inFile, line);
        this->parseLine(line);
    }
}

std::string Mimes::getContentType(std::string& path)
{
    std::string ext = getFileExtension(path);
    if (ext.empty() || this->extensions.find(ext) == this->extensions.end())
        return "text/html";
    std::string extension = this->extensions[ext];
    size_t pos = extension.find("|");
    if (pos != std::string::npos)
        return extension.substr(0, pos);
    return extension;
}

bool Mimes::isMime(std::string& ext)
{
    return this->extensions.find(ext) != this->extensions.end();
}
Mimes::~Mimes()
{

}

// Mimes::Mimes(const Mimes& other)
// {

// }

// Mimes& Mimes::operator=(const Mimes& other)
// {

// }

