/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/27 10:37:31 by hoigag            #+#    #+#             */
/*   Updated: 2024/01/27 12:23:09 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "helpers.hpp"

std::string readfromFd(int fd)
{
    int r = 1;
    int readSize = 1024;
    char buffer[readSize + 1];
    std::string result = "";
    while (r)
    {
        r = read(fd, buffer, readSize);
        if (r < 0)
            break;
        buffer[r] = '\0';
        std::string line = std::string(buffer);
        result += line;
    }
    close(fd);
    return result;
}

std::string getFileExtension(std::string filename)
{
    std::string ext = "";
    size_t pos = filename.find(".");
    if (pos != std::string::npos)
        ext = filename.substr(pos);
    return ext;
}

bool isSupportedCgiScript(std::string script)
{
    std::string ext = getFileExtension(script);
    return (ext == ".py" || ext == ".php") ? true : false;    
}

std::string loadFile(const std::string& path)
{
    std::ifstream inFile;
    inFile.open(path.c_str());
    if (!inFile.is_open())
        throw std::runtime_error("could not open file");
	std::ostringstream buffer;
	buffer << inFile.rdbuf();
	inFile.close();
	return buffer.str();
}


void parseLine(std::map<std::string, std::string>& extensions, std::string line)
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
            extensions[ext] = contentType;
        }
    }
    // std::cout << extension << std::endl;
}

std::string getContentType(std::string ext)
{
    std::map<std::string, std::string> extensions;
    std::ifstream inFile("mimes.txt");
    if (!inFile.is_open())
        throw std::runtime_error("could not open mimes file");
    while (inFile)
    {
        std::string line;
        std::getline(inFile, line);
        parseLine(extensions, line);
    }
    return extensions[ext];
}


// std::string getContentType(std::string url)
// {
//     std::ifstream mimesFile("mimes.txt");
//     if (!mimesFile)
//         throw std::runtime_error("could not open mime file");
//     while (std::getline(mimesFile))
// }