/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/27 10:37:31 by hoigag            #+#    #+#             */
/*   Updated: 2024/03/12 15:52:20 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "helpers.hpp"


void setSocketToBeReusable(int sock)
{
    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        std::cerr << "Setsockopt error: " << sock << std::endl;
        exit (1);
    }
}

void setSocketToNonBlocking(int socket)
{
    int status = fcntl(socket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
    if (status < 0)
    {
        std::cout << "could not set socket " << socket << "to non blocking" << std::endl;
        exit(1);
    }
}


std::string readfromFd(int fd)
{
    // std::cout << "reading from fd" << std::endl;
    int r = 1;
    int readSize = 1024;
    char buffer[readSize + 1];
    std::string result = "";
    while (r)
    {
        // std::cout << "in loop" << std::endl;
        r = read(fd, buffer, readSize);
        // std::cout << "r == " << r << std::endl;
        if (r <= 0)
            break;
        buffer[r] = '\0';
        std::string line(buffer, r);
        // std::cout << "line == " << line << std::endl;
        result += line;
    }
    // std::cout << "result == " << result << std::endl;
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
    // std::cout << "path == " << path << std::endl;
    std::ifstream inFile;
    inFile.open(path.c_str());
    if (!inFile.is_open())
    {
        throw std::runtime_error("could not open file load file function");
        
    }
	std::ostringstream buffer;
	buffer << inFile.rdbuf();
	inFile.close();
	return buffer.str();
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


void urlValidator(std::string& url)
{
    for (int i = 0; url[i]; i++)
        if (url[i] == '/' && url[i + 1] && url[i + 1] == '/')
            url.erase(i, 1);
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
    std::string content = "<!DOCTYPE html> <html lang=\"en\" <head>\
    <meta charset=\"UTF-8\">\
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\
    <title>File List</title>\
    <link rel=\"stylesheet\" href=\"/styles/listing.css\">\
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
                std::string directory = path + "/" + filename;
                std::string href = "/" + relativePath + "/" + filename;
                std::string link;
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

void uploadFiles(Request& req)
{
    std::cout << "uploading files" << std::endl;
    vector<s_tuple > data = req.getMultipart();
    std::cout << " -----> req: "<< req.getBody().size() << std::endl;
    
    std::string body = "";
    std::cout << "url === " << req.getURL() << std::endl;
    for (size_t i = 0; i < data.size(); i++)
    {
        if (data[i].fileName.empty())
            body += data[i].name + "=" + data[i].value + "&";
        else
            {
                std::cout << "filename === " << data[i].fileName << std::endl;
                std::ofstream outfile("upload/" + data[i].fileName);
                if (!outfile.is_open())
                    throw std::runtime_error("could not open the file upload");
                if (data[i].value.empty())
                    std::cerr << "the file content is empty" << std::endl;
                else
                {
                    // std::cout << "file content == " << data[i].value.length() << std::endl;
                    // std::cout << "writing to file" << std::endl;
                    outfile.write(data[i].value.data(), data[i].value.size());
                    std::cout << " -----> len: "<< data[i].value.size() << std::endl;
                    // std::cout << "done writing to file" << std::endl;
                    outfile.close();
                }
            }
    }
    // req.setBody(body);
}

std::string sread(int socket)
{
    char dataRead[BUFFER_SIZE + 1];
    int bytesRead = recv(socket, dataRead, BUFFER_SIZE, 0);
    if (bytesRead < 0)
        std::runtime_error("recv error : could not read from socket");
    dataRead[bytesRead] = '\0';
    return std::string(dataRead, bytesRead);
}

// int sendChunk(int sock, ClientResponse& cr)
// {
//     int dataSent = 0;
//     dataSent = send(sock, cr.response.getResponseString().c_str() + cr.totalDataSent, cr.responseSize - cr.totalDataSent, 0);
//     if (dataSent < 0)
//     {   
//         std::cerr << "error: Could not send data" << std::endl;
//         return 0;
//     }
//     cr.totalDataSent += dataSent;
//     if (cr.totalDataSent >= cr.responseSize)
//         cr.isResponseFinished = true;
//     return 1;
// }