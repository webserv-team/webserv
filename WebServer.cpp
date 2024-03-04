/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/25 13:25:34 by hoigag            #+#    #+#             */
/*   Updated: 2024/03/04 18:42:26 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include <iostream>
#include <fstream>
#include "helpers.hpp"
#include "Cgi.hpp"
#include "Response.hpp"



std::vector<char> readBinaryFile(const std::string& filename) {
    std::ifstream file(filename.c_str(), std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return std::vector<char>(); // Return an empty vector on error
    }

    // Read the entire file content into a vector
    std::vector<char> buffer;
    char ch;
    while (file.get(ch)) {
        buffer.push_back(ch);
    }

    file.close();
    return buffer;
}

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

// Response	WebServer::formResponse(Request req)
// {
//     Response response;
//     std::string content;
//     std::string header;
//     std::string contentType;
//     std::string resourceFullPath = this->server.documentRoot;
//     std::string url = req.getURL();
//     size_t pos = url.find("?");
//     if (req.getMethod() == "POST" && req.getContentType().find("multipart/form-data") != string::npos)
//         uploadFiles(req);
//     if (url == "/")
//         url = "/index.html";
//     if (pos != std::string::npos)
//         resourceFullPath += url.substr(0, pos);
//     else
//         resourceFullPath += url;
//     if (access(resourceFullPath.c_str(), F_OK) != 0)    
//     {
//         content = loadFile(this->server.errorPages["not_found"]);
//         response.setStatusCode(404);
//     }
//     else if (access(resourceFullPath.c_str(), R_OK) != 0)
//     {
//         content = loadFile(this->server.errorPages["forbidden"]);
//         response.setStatusCode(403);
//     }
//     else if (isDirectory(resourceFullPath))
//         content = this->directoryListing(resourceFullPath);
//     else if (isSupportedCgiScript(resourceFullPath))
//     {
//         Cgi cgi(req);
//         content = cgi.executeScript(resourceFullPath);
//         size_t pos = content.find("\r\n\r\n");
//         if (pos != std::string::npos)
//         {
//             content = content.substr(pos);
//             header = content.substr(0, pos);
//         }
//     }
//     else
//     {
//         std::vector<char> data = readBinaryFile(this->server.documentRoot + url);
//         std::string res(data.begin(), data.end());
//         content = res;
//         // content = loadFile(this->server.documentRoot + url);
//     }
//     if (!isSupportedCgiScript(url))
//     {
//         std::string ext = getFileExtension(url);
//         contentType = mimes.getContentType(ext);
//     }
//     else if (isSupportedCgiScript(url))
//         contentType = getContentTypeFromCgiOutput(header);
//     response.setContentType(contentType);
//     response.setContentLength(content.size());
//     response.setBody(content);
//     response.buildResponse();
//     // std::cout << response << std::endl;
//     return response;
// }

// WebServer::WebServer(ConfigData ServerConf)
// {
//     this->server = ServerConf;
// }

// void WebServer::createSocket()
// {
//     this->listenFD = socket(AF_INET, SOCK_STREAM, 0);
//     if (this->listenFD < 0)
//         throw std::runtime_error("could not create server socket");
//             int status = fcntl(this->listenFD, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
//             if (status < 0)
//             {
//                 std::cout << "could not set request ot non blocking" << std::endl;
//                 exit(1);
//             }
// }

// void WebServer::bindSocket()
// {
//     int opt = 1;
//     if (setsockopt(this->listenFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
//     {
//         std::cerr << "Setsockopt error: " << strerror(errno) << std::endl;
//         exit (5);
//     }
//     bzero(&this->servaddr, sizeof(this->servaddr));
//     servaddr.sin_family = AF_INET;
//     servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
//     servaddr.sin_port = htons(this->server.port);
//     //*binding the socket to the server
//     if(bind(this->listenFD, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
//         throw std::runtime_error("could not bind to socket");
// }

std::string sread(int socket)
{
    int size = 100000;
    char dataRead[size + 1];
    int bytesRead = recv(socket, dataRead, size, 0);
    if (bytesRead < 0)
        std::runtime_error("recv error : could not read from socket");
    dataRead[bytesRead] = '\0';
    std::string str(dataRead, bytesRead);
    return str;
}

void handlePostRequest(Header& header, Client& client, std::string& dataRead)
{
    // std::cout << "handling post request" << std::endl;
    if (dataRead.find("\r\n\r\n") == std::string::npos)
    {
        client.content.append(dataRead);
        client.bytesRead += dataRead.length();
    }
    
    // std::cout << "content length == " << header.getContentLength() << std::endl;
    // std::cout << "bytes read == " << client.bytesRead << std::endl;
    // // if (client.bytesRead == header.getContentLength())
    // std::cout << "data read = " << dataRead << std::endl;
    if (client.bytesRead >= header.getContentLength())
    {
        std::cout << "POST REQUEST END" << std::endl;
        client.isRequestFinished = true;
        // std::cout << "conetnt = " << client.content << std::endl;
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
    return 1;
}

// void WebServer::listenForConnections()
// {
//     while(1)
//     {
        
//     }
// }


WebServer::WebServer(std::vector<Socket>& httpServers) 
{
    this->httpServers = httpServers;
    this->maxFd = 0;
    for (size_t i = 0; i < this->httpServers.size(); i++)
    {
        this->httpServers[i].start();
        FD_ZERO(&this->read_sockets);
        FD_ZERO(&this->write_sockets);
        int serverFD = this->httpServers[i].getFd();
        FD_SET(serverFD, &read_sockets);
        if (serverFD > this->maxFd)
            this->maxFd = serverFD;  
    }
    
}


void WebServer::listenForConnections()  
{
    // int connFd;
    while (true)
    {
        this->read_copy_sockets = this->read_sockets;
        this->write_copy_sockets = this->write_sockets;
        if (select(this->maxFd + 1, &this->read_copy_sockets, &this->write_copy_sockets, NULL, NULL) < 0)
        {
            std::cerr << "error occured on select" << std::endl;
            exit(1);
        }

    //     for (int i = 0; i <= this->maxFd; i++)
    //     {
    //         if (FD_ISSET(i, &read_copy_sockets))
    //         {
    //             if (i == this->listenFD)
    //             {
    //                 connFd = accept(this->listenFD,  (struct sockaddr *)&addr, (socklen_t *) &addr_len);
    //                 if (connFd < 0)
    //                 {
    //                     std::cout << "could not accept the connection" << std::endl;
    //                     exit(1);
    //                 }
    //                 int status = fcntl(connFd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
    //                 if (status < 0)
    //                 {
    //                     std::cout << "could not set request ot non blocking" << std::endl;
    //                     exit(1);
    //                 }
                        
    //                 FD_SET(connFd, &read_sockets);
    //                 if (connFd > maxFd)
    //                     maxFd = connFd;
    //                 Client c = {"" , false, 0, 0, "", "", false};
    //                 this->clients[connFd] = c;
    //             }
    //             else
    //             {    
    //                 std::cout << i << std::endl;
    //                 std::string dataRead = sread(i);
    //                 Header headers;
    //                 size_t carr_pos = dataRead.find("\r\n\r\n");
    //                 if (carr_pos != std::string::npos)
    //                 {
    //                     this->clients[i].isHeaderFinished = true; 
    //                     this->clients[i].header += dataRead.substr(0, carr_pos);
    //                     this->clients[i].content = dataRead.substr(carr_pos + 4);
    //                     this->clients[i].bytesRead = this->clients[i].content.length();
    //                 }
    //                 if (this->clients[i].isHeaderFinished)
    //                 {
    //                     headers =  Header(this->clients[i].header);   
    //                     // std::cout << "header is finished " << headers.getMethod() << std::endl;
    //                     if (headers.getMethod() == "GET")
    //                         this->clients[i].isRequestFinished = true;
    //                     else if (headers.getMethod() == "POST")
    //                         handlePostRequest(headers, this->clients[i], dataRead);
    //                 }
    //                 else
    //                     this->clients[i].header.append(dataRead);
    //                 if (this->clients[i].isRequestFinished)
    //                 {
    //                     FD_CLR(i, &read_sockets);
    //                     FD_SET(i, &write_sockets);
    //                     std::string httprequest = this->clients[i].header + "\r\n\r\n" + this->clients[i].content;
    //                     // std::cout << httprequest << std::endl<<std::endl;
    //                     Request req(httprequest);
    //                     this->clientResponses[i].response = this->formResponse(req);
    //                     this->clientResponses[i].responseSize = this->clientResponses[i].response.getResponseLength();
    //                     this->clientResponses[i].dataSent = 0;
    //                     this->clientResponses[i].totalDataSent = 0;
    //                     this->clientResponses[i].isResponseFinished = false;
    //                 }
    //             }
    //         }
    //         if (FD_ISSET(i, &write_copy_sockets))
    //         {
    //             int success = sendChunk(i, this->clientResponses[i]);
    //             if (!success)
    //             {
    //                 FD_CLR(i, &write_sockets);
    //                 close(i);
    //             }
    //             if (this->clientResponses[i].totalDataSent >= this->clientResponses[i].responseSize)
    //             {
    //                 FD_CLR(i, &write_sockets);
    //                 // FD_SET(i, &read_sockets);
    //                 close(i);
    //             }
    //         }
    //     }
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

