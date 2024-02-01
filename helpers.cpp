/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/27 10:37:31 by hoigag            #+#    #+#             */
/*   Updated: 2024/02/01 16:02:05 by hoigag           ###   ########.fr       */
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
        
    if (content.empty())
        return "text/html";
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
    size_t pos = path.find("/");
    std::string relativePath;
    if (pos != std::string::npos)
        relativePath = path.substr(pos + 1);
    // std::string path = "/Users/hoigag/cursus/webserv/htdocs/" + path;
    std::cout << "relative directory path = " << path << std::endl;
    std::cout << "absolute directory path = " << path << std::endl;
    // if (chdir(path.c_str()) < 0)
    //     return ("could not change to " + path);
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
        .directory {\
            text-decoration: none;\
            color: #4E148C;\
            font-weight: bold;\
        }\
        .icon {\
            margin-right: 5px;\
        }\
        a:hover {\
            color: #0056b3;\
        }\
    </style>\
</head>\
<body>\
    <h1>Directory Listing</h1>\
    <hr>\
    <ul>";
    
    struct dirent *stdir;
    DIR *dir = opendir(path.c_str());
    if (!dir)
        return  ("<p>could not open the directory " + path + " </p>");
    
    content += "<li><strong>Directory: " + path + "</strong></li><br>";

    stdir = readdir(dir);
    while (stdir)
    {
        if (stdir)
        {   
            std::string filename(stdir->d_name);
            if (filename != "." && filename != "..")
            {
                std::string href = "/" + relativePath + "/" + filename;
                std::string link;
                std::string directory = path + "/" + filename;
                std::cout << "directory = " << directory << std::endl;
                if (isDirectory(directory))
                    link += "<li><span class=\"icon\">📁</span><a href=\"" + href + "\" class=\"directory\">" + filename + "</a></li>";
                else
                    link += "<li><span class=\"icon\">📄</span><a href=\"" + href + "\" class=\"file\">" + filename + "</a></li>";
                content += link;
            }
        }
        stdir = readdir(dir);
    }
    content += "</ul></body></html>";
    closedir(dir);

    return content; 
}