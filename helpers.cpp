/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/27 10:37:31 by hoigag            #+#    #+#             */
/*   Updated: 2024/01/30 18:43:42 by hoigag           ###   ########.fr       */
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


void parseLine(std::map<std::string, std::string>& extensions, std::string& line)
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

std::string getContentTypeFromCgiOutput(std::string& content)
{
    std::stringstream stream;
    std::string line;
    stream.str(content);
    std::getline(stream, line);
    std::getline(stream, line);
    size_t semiPos = line.find(";");
    std::string header = line.substr(0, semiPos);
    size_t colonPos = header.find(":");
    std::string contentType = header.substr(colonPos + 2);
    return contentType;
}

bool isDirectory(std::string& path)
{
    struct stat info;
    if (stat(path.c_str(), &info) < 0)
    {
        std::cerr << "could not open file : " << path << std::endl;
        return false;
    }
    return S_ISDIR(info.st_mode);
}
std::string directoryListing(std::string& path)
{
    std::string content = "<!DOCTYPE html> <html lang=\"en\" <head>\
    <meta charset=\"UTF-8\">\
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\
    <title>File List</title>\
    <style>\
        body {\
            font-family: Arial, sans-serif;\
            margin: 20px;\
        }\
        h1 {\
            color: #333;\
        }\
        ul {\
            list-style-type: none;\
            padding: 0;\
        }\
        li {\
            margin-bottom: 10px;\
        }\
        .file {\
            text-decoration: none;\
            color: #007BFF;\
            font-weight: bold;\
        }\
        .direcotry\
        {\
            text-decoration: none;\
            color: #ff0000;\
            font-weight: bold;\
        }\
        a:hover {\
            color: #0056b3;\
        }\
    </style>\
</head>\
<body>\
    <h1>File List</h1>\
    <ul>";
    struct dirent *stdir;

    DIR *dir = opendir(path.c_str());
    if (!dir)
    {
        content = "<p>could not open the directory " + path + " </p>";
        return content;
    }
    size_t pos = path.find("/");
    if (pos != std::string::npos)
        path = path.substr(pos + 1);
            std::cout << "path after skipping = " << path <<std::endl;
    stdir = readdir(dir);
    while (stdir)
    {
        if (stdir)
        {   
            std::string dirname(stdir->d_name);
            // std::cout << "<p>" << dirname << "</p>" << std::endl;
            std::cout << "file name = " << dirname << std::endl;
            std::string href = path + "/" + dirname;
            std::cout << "href = " << href << std::endl;
            std::string link = "<li>";
                link += "<a href=\"" + href + "\">" + dirname + "</a></li>";
            std::cout << "tag = " << link << std::endl;
            content += link;
        }
        stdir = readdir(dir);
    }
    content += "</ul></body></html>";
    closedir(dir);
    // std::cout << content << std::endl;
    return content;
}