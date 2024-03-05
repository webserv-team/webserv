/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/25 13:25:34 by hoigag            #+#    #+#             */
/*   Updated: 2024/03/05 17:40:48 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include <iostream>
#include <fstream>
#include "helpers.hpp"
#include "Cgi.hpp"
#include "Response.hpp"

class Header
{
    public:
        Header() : req(""){};
        Header(std::string& req){this->req = req;}
        std::string getMethod()
        {
            size_t pos = this->req.find(" ");
            return this->req.substr(0, pos);
        }
        int getContentLength()
        {
            int contentlength = 0;
            size_t pos = this->req.find("Content-Length: ");
            if (pos != std::string::npos)
            {
                std::string length = req.substr(pos + 16);
                contentlength = atoi(length.c_str());
            }
            return contentlength;
        }
        bool isChunked()
        {
            size_t pos = this->req.find("Transfer-Encoding: chunked");
            if (pos != std::string::npos)
                return true;
            return false;
        }
    private:
        std::string req;
};


// WebServer::WebServer()
// {
    
// }

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
    std::cout << "uploading files" << std::endl;
    vector<s_tuple > data = req.getMultipart();
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
                    outfile << data[i].value;
            }
    }
    // req.setBody(body);
}

Socket WebServer::getServer(int port)
{
    int index = -1;
    for (size_t i = 0; i < this->httpServers.size(); i++)
    {
        if (this->httpServers[i].getConfData().port == port)
            index = i;
    }
    return this->httpServers[index];
}

Response	WebServer::formResponse(Request __unused req)
{
    Response response;
    std::string content;
    std::string header;
    std::string contentType;
    ConfigData conf = this->getServer(req.getPort()).getConfData();
    std::string resourceFullPath = conf.documentRoot;
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
        content = loadFile(conf.errorPages["not_found"]);
        response.setStatusCode(404);
    }
    else if (access(resourceFullPath.c_str(), R_OK) != 0)
    {
        content = loadFile(conf.errorPages["forbidden"]);
        response.setStatusCode(403);
    }
    else if (isDirectory(resourceFullPath))
        content = this->directoryListing(resourceFullPath);
    else if (isSupportedCgiScript(resourceFullPath))
    {
        Cgi cgi(req);
        content = cgi.executeScript(resourceFullPath);
        size_t pos = content.find("\r\n\r\n");
        if (pos != std::string::npos)
        {
            content = content.substr(pos);
            header = content.substr(0, pos);
        }
    }
    else
        content = loadFile(conf.documentRoot + url);
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
    response.buildResponse();
    return response;
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

void handlePostRequest(Header& header, Client& client, std::string& dataRead)
{
    if (dataRead.find("\r\n\r\n") == std::string::npos)
    {
        client.content.append(dataRead);
        client.bytesRead += dataRead.length();
    }
    if (client.bytesRead >= header.getContentLength())
    {
        std::cout << "POST REQUEST END" << std::endl;
        client.isRequestFinished = true;
    }
}

int sendChunk(int sock, ClientResponse& cr)
{
    int dataSent = 0;
    dataSent = send(sock, cr.response.getResponseString().c_str() + cr.totalDataSent, cr.responseSize - cr.totalDataSent, 0);
    if (dataSent < 0)
    {   
        std::cerr << "error: Could not send data" << std::endl;
        return 0;
    }
    cr.totalDataSent += dataSent;
    if (cr.totalDataSent >= cr.responseSize)
        cr.isResponseFinished = true;
    return 1;
}

WebServer::WebServer(std::vector<Socket>& httpServers) 
{
    FD_ZERO(&this->read_sockets);
    FD_ZERO(&this->write_sockets);
    this->httpServers = httpServers;
    this->maxFd = 0;
    for (size_t i = 0; i < this->httpServers.size(); i++)
    {
        this->httpServers[i].start();
        int serverFD = this->httpServers[i].getFd();
        FD_SET(serverFD, &read_sockets);
        if (serverFD > this->maxFd)
            this->maxFd = serverFD;  
    }
}

bool WebServer::isServerFd(int fd)
{
    for (size_t i = 0; i < this->httpServers.size(); i++)
    {
        if (this->httpServers[i].getFd() == fd)
            return true;
    }
    return false;
}

void WebServer::handleNewConnection(int serverFd)
{
    int connFd = Socket::acceptNewConnetction(serverFd);
    setSocketToNonBlocking(connFd);
    FD_SET(connFd, &this->read_sockets);
    if (connFd > this->maxFd)
        this->maxFd = connFd;
    Client newClient = {"" , false, 0, 0, "", "", false};
    this->clients[connFd] = newClient;
}

void WebServer::handleExistingConnection(int fd)
{
    std::string dataRead = sread(fd);
    Header headers;
    size_t carr_pos = dataRead.find("\r\n\r\n");
    if (carr_pos != std::string::npos)
    {
        this->clients[fd].isHeaderFinished = true; 
        this->clients[fd].header += dataRead.substr(0, carr_pos);
        this->clients[fd].content = dataRead.substr(carr_pos + 4);
        this->clients[fd].bytesRead = this->clients[fd].content.length();
    }
    if (this->clients[fd].isHeaderFinished)
    {
        headers =  Header(this->clients[fd].header);   
        if (headers.getMethod() == "GET")
            this->clients[fd].isRequestFinished = true;
        else if (headers.getMethod() == "POST")
            handlePostRequest(headers, this->clients[fd], dataRead);
    }
    else
        this->clients[fd].header.append(dataRead);
    if (this->clients[fd].isRequestFinished)
    {
        FD_CLR(fd, &read_sockets);
        FD_SET(fd, &write_sockets);
        std::string httprequest = this->clients[fd].header + "\r\n\r\n" + this->clients[fd].content;
        Request req(httprequest);
        std::cout << req;
        this->clientResponses[fd].response = this->formResponse(req);
        this->clientResponses[fd].responseSize = this->clientResponses[fd].response.getResponseLength();
        this->clientResponses[fd].dataSent = 0;
        this->clientResponses[fd].totalDataSent = 0;
        this->clientResponses[fd].isResponseFinished = false;
    }
}

void WebServer::listenForConnections()  
{
    while (true)
    {
        this->read_copy_sockets = this->read_sockets;
        this->write_copy_sockets = this->write_sockets;
        if (select(this->maxFd + 1, &this->read_copy_sockets, &this->write_copy_sockets, NULL, NULL) < 0)
        {
            std::cerr << "error occured on select" << std::endl;
            exit(1);
        }
        for (int i = 0; i <= this->maxFd; i++)
        {
            if (FD_ISSET(i, &this->read_copy_sockets))
            {
                if (this->isServerFd(i))
                    this->handleNewConnection(i);
                else
                    this->handleExistingConnection(i);
            }
            if (FD_ISSET(i, &this->write_copy_sockets))
            {
                int success = sendChunk(i, this->clientResponses[i]);
                if (!success)
                {
                    FD_CLR(i, &this->write_sockets);
                    close(i);
                }
                if (this->clientResponses[i].isResponseFinished)
                {
                    FD_CLR(i, &this->write_sockets);
                    close(i);
                }
            }
        }
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

