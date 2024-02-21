/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/25 13:25:34 by hoigag            #+#    #+#             */
/*   Updated: 2024/02/21 18:17:57 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include <iostream>
#include <fstream>
#include "helpers.hpp"
#include "Cgi.hpp"
#include "Response.hpp"

using namespace std;


std::string WebServer::directoryListing(std::string& path)
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
    vector<s_tuple > data = req.getMultipart();
    std::string body = "";
    for (size_t i = 0; i < data.size(); i++)
    {
        if (data[i].fileName.empty())
            body += data[i].name + "=" + data[i].value + "&";
        else
            {
                std::ofstream outfile("upload/" + data[i].fileName);
                if (!outfile.is_open())
                    throw std::runtime_error("could not open the file");
                if (data[i].value.empty())
                    std::cerr << "the file content is empty" << std::endl;
                else
                    outfile << data[i].value;
            }
    }
    // req.setBody(body);
    std::cout << "body === " << body << std::endl;
}

void	WebServer::sendResponse(Request req, int sock)
{
    Response response;
    std::string content;
    std::string header;
    std::string contentType;
    std::string resourceFullPath = this->server.documentRoot;
    std::string url = req.getURL();
    size_t pos = url.find("?");
    if (req.getMethod() == "POST" && req.getContentType().find("multipart/form-data") != string::npos)
        uploadFiles(req);
    if (url == "/")
        url = "/index.html";
    if (pos != std::string::npos)
        resourceFullPath += url.substr(0, pos);
    else
        resourceFullPath += url;
    if (access(resourceFullPath.c_str(), F_OK) != 0)    
    {
        content = loadFile(this->server.errorPages["not_found"]);
        response.setStatusCode(404);
    }
    else if (access(resourceFullPath.c_str(), R_OK) != 0)
    {
        content = loadFile(this->server.errorPages["forbidden"]);
        response.setStatusCode(403);
    }
    else if (isDirectory(resourceFullPath))
        content = this->directoryListing(resourceFullPath);
    else if (isSupportedCgiScript(resourceFullPath))
    {
        // Cgi cgi(req);
        // content = cgi.executeScript(resourceFullPath);
        // size_t pos = content.find("\r\n\r\n");
        // if (pos != std::string::npos)
        // {
        //     content = content.substr(pos);
        //     header = content.substr(0, pos);
        // }
        content = "cgi script";
    }
    else
        content = loadFile(this->server.documentRoot + url);
    if (!isSupportedCgiScript(url))
    {
        std::string ext = getFileExtension(url);
        contentType = mimes.getContentType(ext);
    }
    else if (isSupportedCgiScript(url))
        contentType = getContentTypeFromCgiOutput(header);
    response.setContentType(contentType);
    response.setContentLength(content.size());
    response.setBody(content);
    // std::cout << response << std::endl;
    response.sendIt(sock);
}

WebServer::WebServer(ConfigData ServerConf)
{
    this->server = ServerConf;
}

void WebServer::createSocket()
{
    this->listenFD = socket(AF_INET, SOCK_STREAM, 0);
    if (this->listenFD < 0)
        throw std::runtime_error("could not create server socket");
}

void WebServer::bindSocket()
{
    int opt = 1;
    if (setsockopt(this->listenFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        std::cerr << "Setsockopt error: " << strerror(errno) << std::endl;
        exit (5);
    }
    bzero(&this->servaddr, sizeof(this->servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(0x7f000001);
    servaddr.sin_port = htons(this->server.port);

    //*binding the socket to the server
    if(bind(this->listenFD, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
        throw std::runtime_error("could not bind to socket");
}
//  std::string getRequest(int connFd)
//  {
//     char data[REQUEST_LENGTH];
//     int n = read(connFd, data, REQUEST_LENGTH);
//     if (n  < 0)
//     {
//         std::cout << "could not read the request" << std::endl;
//         exit (1);
//     }
//     data[n] = '\0';
//     std::string request(data);
//     return request;
//  }
std::string getRequest(int socket)
{
    int bufferSize = 1024;
    
    char buffer[bufferSize + 1];
    int r = 1;
    std::string request = "";
    size_t cpos;
    int bodySizeReceived = 0;
    while(r > 0)
    {
        r = recv(socket, buffer, bufferSize, 0);
        if (r < 0)
            break;
        buffer[r] = '\0';
        std::string sbuffer(buffer);
        request += sbuffer;
        if (sbuffer.find("\r\n\r\n") != std::string::npos)
            break;
    }
    if (request.substr(0, request.find(" ")) == "POST")
    {
        cpos = request.find("Content-Length");
        int contentlength = 0;
        if (cpos != std::string::npos)
            contentlength = atoi(request.substr(cpos + 15, request.find("\r\n")).c_str());
        size_t pos = request.find("\r\n\r\n");
        std::string header = request.substr(0, pos);
        std::string body = request.substr(pos + 4);
        bodySizeReceived = body.length();
        while (bodySizeReceived < contentlength)
        {
            r = recv(socket, buffer, bufferSize, 0);
            if (r < 0)
                break;
            buffer[r] = '\0';
            std::string sbuffer(buffer);
            body += sbuffer;
            bodySizeReceived += r;
        }
        std::cout << "body received " << bodySizeReceived << " | content LENGTH = " << contentlength << std::endl;
    }
    return request;
}

std::string getMethod(std::string& req)
{
    size_t pos = req.find(" ");
    if (pos != std::string::npos)
        return req.substr(0, pos);
    return "";
}

std::string sread(int socket)
{
    char dataRead[BUFFER_SIZE + 1];
    int bytesRead = recv(socket, dataRead, BUFFER_SIZE, 0);
    if (bytesRead < 0)
        std::runtime_error("recv error : could not read from socket");
    dataRead[bytesRead] = '\0';
    std::string str(dataRead);
    return str;
}

int getContentLength(std::string& req)
{
    int contentlength = 0;
    size_t pos = req.find("Content-Length: ");
    if (pos != std::string::npos)
    {
        std::string length = req.substr(pos + 16);
        std::cout << "len len len === " << length << std::endl;
        contentlength = atoi(length.c_str());
    }
    return contentlength;
}

void WebServer::listenForConnections()
{
    int connFd;
    // std::string html = loadFile("index.html");
    if ((listen(this->listenFD, 3)) < 0)
        std::runtime_error("error happened during listening for requests");
    fd_set current_sockets, ready_sockets;
    FD_ZERO(&current_sockets);
    FD_SET(this->listenFD, &current_sockets);
    while (true)
    {
        ready_sockets = current_sockets;
        if (select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL) < 0)
            std::runtime_error("select error");
        struct sockaddr_in addr;
        socklen_t addr_len;
        for (int i = 0; i < FD_SETSIZE; i++)
        {
            if (FD_ISSET(i, &ready_sockets))
            {
                if (i == this->listenFD)
                {
                    std::cout << "new connection" << std::endl;
                    connFd = accept(this->listenFD,  (struct sockaddr *)&addr, (socklen_t *) &addr_len);
                    if (connFd < 0)
                    {
                        std::cout << "could not accept the connection" << std::endl;
                        exit(1);
                    }
                   FD_SET(connFd, &current_sockets);
                //    std::cout << "partial request: " << this->clients[connFd] << std::endl;
                }
                else
                {    
                    std::cout << "already existing client" << std::endl;
                    // this->clients[i] += sread(i);
                    // this->clients[i] += sread(i); 
                    std::string request = this->clients[i];
                    if (getMethod(request) == "GET" && request.find("\r\n\r\n") != std::string::npos)
                    {
                        std::cout << "<" << getMethod(request) << ">" << std::endl;
                        std::cout << request << std::endl;
                        Request req(request);
                        std::cout << "sending response" << std::endl;
                        // sendResponse(req, i);
                        // close(i);
                        FD_CLR(i, &current_sockets);
                    }
                    else if (getMethod(request) == "POST")
                    {
                        std::cout << "POST REQUEST" << std::endl;
                        std::cout << this->clients[i] << std::endl;
                        std::cout << "content length = " << getContentLength(request) << std::endl;
                        FD_CLR(i, &current_sockets);
                    }
                    else
                    {
                        this->clients[i] += sread(i);
                        std::cout << this->clients[i] << std::endl;
                    }
                }
            }
        }
        // std::cout << "listening on port " << this->server.port << std::endl;

         
        // std::string request = getRequest(connFd);
        // if (request.empty())
        //     continue;
            // Request req(request);
        
        // try
        // {
        //     sendResponse(req, connFd);
        // }
        // catch(const std::exception& e)
        // {
        //     std::cerr << "error sending the response" << '\n';
        //     close(connFd);
        // }
        
    }
}

WebServer::~WebServer()
{

}



// WebServer::WebServer(const WebServer& other)
// {

// }

// WebServer& WebServer::operator=(const WebServer& other)
// {

// }

