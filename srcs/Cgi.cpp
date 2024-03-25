/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/25 10:51:02 by hoigag            #+#    #+#             */
/*   Updated: 2024/03/25 22:50:08 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"
Cgi::Cgi()
{
    
}


Cgi::Cgi(Request& req, Location& __unused location)
{
    
    this->req = req;
    std::string path;
    // this->REQUEST_URI = req.getURL();
    this->vars["REQUEST_METHOD"] = req.getMethod();
    if (this->vars["REQUEST_METHOD"] == "GET")
    {
        size_t pos = req.getURL().find("?");
        if (pos != std::string::npos)
        {
            this->vars["QUERY_STRING"] = req.getURL().substr(pos + 1);
            path = req.getURL().substr(0, pos);
        }
        else
            path = req.getURL();
    }
    this->vars["DOCUMENT_ROOT"] = "htdocs";
    this->vars["REQUEST_URI"] = req.getURL();
    this->vars["REDIRECT_STATUS"] = "200";
    this->vars["SCRIPT_NAME"] = path.substr(path.find_last_of("/"));
    this->vars["SCRIPT_FILENAME"] = location.root + path;
    if (req.getContentType() != "")
        this->vars["CONTENT_TYPE"] = req.getContentType();
    if (req.getContentLength() > 0)
        this->vars["CONTENT_LENGTH"] = std::to_string(req.getContentLength());  
    this->vars["SERVER_SOFTWARE"] = "webserv/1.0";
	this->vars["GATEWAY_INTERFACE"] = "CGI/1.1";
    this->vars["SERVER_PORT"] = req.getPort();
    this->setEnv();
}

void Cgi::setEnv()
{
    int size = this->vars.size();
    this->env = new char *[size + 1];
    std::map<std::string, std::string>::iterator it = this->vars.begin();
    int i = 0;
    while (it != this->vars.end())
    {
        this->env[i] = NULL;
        std::string value = it->first + "=" + it->second;
        this->env[i] = new char[value.size() + 1];
        strcpy(this->env[i], value.c_str());
        i++;
        it++;
    }
    this->env[size] = NULL;
}



std::string Cgi::executeScript(std::string script)
{
    std::string output = "error";
    const char *command[3];
    if (getFileExtension(script) == ".py")
        command[0] = strdup("/Users/hoigag/.brew/bin/python3");
    else
        command[0] = strdup("htdocs/cgi-bin/php-cgi");
    command[1] = script.c_str();
            std::cout << "command[0] " << command[0] << std::endl;
        std::cout << "command[1] " << command[1] << std::endl;
    std::cout << "the file being exectuted " << command[1] << std::endl;
    command[2] = NULL;
    int pipes[2];
    if (pipe(pipes) < 0)
    {
        std::cerr << "could not pipe" << std::endl;
        exit(1);
    }
    int pid = fork();
    if (pid < 0)
    {
        std::cerr << "problem forking" << std::endl;
        exit(1);
    }
    else if (pid == 0)
    {
        
        if (this->vars["REQUEST_METHOD"] == "POST")
            if (dup2(pipes[0], 0) < 0)
            {
                std::cerr << "error while duppingg" << std::endl;
                exit(1);
            }
        if (dup2(pipes[1], 1) < 0)
        {
            std::cerr << "error while duppingg" << std::endl;
            exit(1);
        }
        // if (dup2(pipes[1], 2) < 0)
        // {
        //     std::cerr << "error while duppingg" << std::endl;
        //     exit(1);
        // }
        if (close(pipes[0]) < 0 || close(pipes[1]) < 0)
        {
            std::cerr << "error while closing pipes" << std::endl;
            exit(1);
        }
        if (execve(command[0], (char * const *)command, this->env) < 0)
        {
            std::cerr << "error while executing the cgi script" << std::endl;
            exit(1);
        }
    }
    else
    {
        if (this->vars["REQUEST_METHOD"] == "POST")
        {
            if (this->req.getBody().length() == 0)
                exit(125);
            // send data in chunks
            std::cout << "sending data in chunks" << std::endl; 
            int totalDataSent = 0;
            while (1)
            {
                if (totalDataSent >= this->req.getContentLength())
                    break;
                std::cout << "before writing to pipe" << std::endl;
                int dataSent = write(pipes[1], this->req.getBody().c_str() + totalDataSent , this->req.getBody().size() - totalDataSent);
                std::cout << "after writing to pipe" << std::endl;
                std::cout << "dataSent == " << dataSent << std::endl;
                if (dataSent < 0)
                {
                    std::cerr << "error while writing to pipe" << std::endl;
                    exit(1);
                }
                totalDataSent += dataSent;
            }
            std::cout << "finished sending data in chunks" << std::endl;
            // write(pipes[1], this->req.getBody().c_str(), this->req.getContentLength());
        }
        close(pipes[1]);
        waitpid(pid, NULL, 0);
        output = readfromFd(pipes[0]);
        close(pipes[0]);
        // std::cout << "output from cgi script " << output << std::endl;
    }
    return output;
}

Cgi::~Cgi()
{
    if (!this->env)
        return;
    for (int i = 0; this->env[i]; i++)
    {
        if (this->env[i])
            delete[] this->env[i]; // Corrected to delete[] for char*
    }
    delete[] this->env;
}
std::string Cgi::getServerPort()
{
    return this->SERVER_PORT;
}
std::string Cgi::getQueryString()
{
    return this->QUERY_STRING;
}
std::string Cgi::getRequestUri()
{
    return this->REQUEST_URI;
}
std::string Cgi::getRequestMethod()
{
    return this->REQUEST_METHOD;
}

std::map<std::string, std::string> Cgi::getVars()
{
    return this->vars;
}

// Cgi::Cgi(const Cgi& other)
// {

// }

// Cgi& Cgi::operator=(const Cgi& other)
// {

// }


// username=asdfasdf&password=adsfadsf