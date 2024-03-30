/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/25 10:51:02 by hoigag            #+#    #+#             */
/*   Updated: 2024/03/30 22:05:28 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"


Cgi::Cgi()
{
    
}


Cgi::Cgi(Request& req, Location& location)
{
    this->req = req;
    this->cgiPath = location.cgiPath;
    std::string path;
    this->vars["REQUEST_METHOD"] = req.getMethod();
    size_t pos = req.getURL().find("?");
    if (pos != std::string::npos)
    {
        this->vars["QUERY_STRING"] = req.getURL().substr(pos + 1);
        path = req.getURL().substr(0, pos);
    }
    else
        path = req.getURL();
    this->vars["DOCUMENT_ROOT"] = location.root;
    this->vars["REQUEST_URI"] = req.getURL();
    this->vars["REDIRECT_STATUS"] = "200";
    if (req.getURL().back() == '/')
        path += location.index;
    this->vars["SCRIPT_NAME"] = path.substr(path.find_last_of("/"));
    this->vars["SCRIPT_FILENAME"] = location.root + path;
    this->vars["CONTENT_TYPE"] = req.getContentType();
    this->vars["CONTENT_LENGTH"] = itoa(req.getContentLength());  
    if (req.isChunked())
        this->vars["CONTENT_LENGTH"] = itoa(req.getBody().size());
    this->vars["SERVER_SOFTWARE"] = "webserv/1.0";
	this->vars["GATEWAY_INTERFACE"] = "CGI/1.1";
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



CgiParser Cgi::executeCgiScript()
{
    std::string output;
    
    time_t start = time(0);
    const char *command[3];
    command[0] = this->cgiPath.c_str();
    command[1] = this->vars["SCRIPT_FILENAME"].c_str();
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
            write(pipes[1], this->req.getBody().c_str(), this->req.getBody().size());
        close(pipes[1]);
        int ret = 0;
        while (ret == 0)
        {
            time_t now = time(0);
            if (now - start > 15)
            {
                kill(pid, SIGKILL);
                break;
            }
            ret = waitpid(pid, NULL, WNOHANG);
        }
        if (ret == 0)
            output  = "timeout occured while executing the cgi script";
        else
            output = readfromFd(pipes[0]);
        close(pipes[0]);
    }
    return CgiParser(output);
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