/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/25 10:51:02 by hoigag            #+#    #+#             */
/*   Updated: 2024/01/27 10:59:44 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"
Cgi::Cgi()
{
    
}


Cgi::Cgi(Request& req)
{
    this->req = req;
    // std::cout << "body : " << req.getBody() << std::endl;
    // this->REQUEST_URI = req.getURL();
    this->vars["REQUEST_URI"] = req.getURL();
    this->vars["REDIRECT_STATUS"] = "200";
    this->vars["SCRIPT_NAME"] = "htdocs/submit.php";
    this->vars["SCRIPT_FILENAME"] = "/Users/hoigag/cursus/webserv/htdocs/submit.php";    
    // this->REQUEST_METHOD = req.getMethod();
    this->vars["REQUEST_METHOD"] = req.getMethod();
    std::map<std::string, std::string> headers = req.getHeaders();
    std::string host = headers.find("Host")->second;
    size_t pos = host.find(":");
    // this->SERVER_PORT = host.substr(pos + 1);
    this->vars["SERVER_PORT"] = host.substr(pos + 1);
    if (this->vars["REQUEST_METHOD"] == "GET")
    {
        size_t pos = req.getURL().find("?");
        if (pos != std::string::npos)
            this->vars["QUERY_STRING"] = req.getURL().substr(pos + 1);
        else
            this->vars["QUERY_STRING"] = "";
            // this->QUERY_STRING = req.getURL().substr(pos + 1);
    }
    else if (this->vars["REQUEST_METHOD"] == "POST")
        this->vars["QUERY_STRING"] = ""; 
        // this->QUERY_STRING = req.getBody();

    // std::cout << "REQUEST_URI" << "=" << this->vars["REQUEST_URI"] << std::endl;
    // std::cout << "REQUEST_METHOD" << "=" << this->vars["REQUEST_METHOD"] << std::endl;
    // std::cout << "SERVER_PORT" << "=" << this->vars["SERVER_PORT"] << std::endl;
    // std::cout << "QUERY_STRING" << "=" << this->vars["QUERY_STRING"] << std::endl;

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
        std::strcpy(this->env[i], value.c_str());
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
        command[0] = "/Users/hoigag/.brew/bin/python3";
    else
        command[0] = "/Users/hoigag/cursus/webserv/htdocs/cgi/php-cgi";
    command[1] = script.c_str();
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
        {
            char msg[this->req.getContentLength() + 1];
            read(pipes[0], msg, this->req.getContentLength());
            msg[this->req.getContentLength()] = '\0';
            // std::cout << "read from input: " << msg << std::endl;
            this->vars["QUERY_STRING"] = msg;
        }
        this->setEnv();
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
            exit(1);
        }
    }
    else
    {
        // close(pipes[0]);
        write(pipes[1], this->req.getBody().c_str(), this->req.getContentLength());
        close(pipes[1]);
        waitpid(pid, NULL, 0);
        output = readfromFd(pipes[0]);
    }
    return output;
}
// std::ostream& operator<<(std::ostream& stream, Cgi& cgi)
// {
//     std::map<std::string, std::string>::iterator it = cgi.getVars().begin();

//         stream << it->first<< std::endl;

//     // stream << "REQUEST_URI=" << cgi.getRequestUri() << std::endl;
//     // stream << "REQUEST_METHOD=" << cgi.getRequestMethod() << std::endl;
//     // stream << "SERVER_PORT=" << cgi.getServerPort() << std::endl;
//     // stream << "QUERY_STRING=" << cgi.getQueryString() << std::endl;
//     return stream;
// }

Cgi::~Cgi()
{

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

