/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/25 10:51:02 by hoigag            #+#    #+#             */
/*   Updated: 2024/01/25 18:43:25 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"
Cgi::Cgi()
{}
Cgi::Cgi(Request& req)
{
    this->req = req;
    std::cout << req;
    // std::cout << "body : " << req.getBody() << std::endl;
    // this->REQUEST_URI = req.getURL();
    this->vars["REQUEST_URI"] = req.getURL();
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
            // this->QUERY_STRING = req.getURL().substr(pos + 1);
    }
    else if (this->vars["REQUEST_METHOD"] == "POST")
        this->vars["QUERY_STRING"] = req.getBody(); 
        // this->QUERY_STRING = req.getBody();

    // std::cout << "REQUEST_URI" << "=" << this->vars["REQUEST_URI"] << std::endl;
    // std::cout << "REQUEST_METHOD" << "=" << this->vars["REQUEST_METHOD"] << std::endl;
    // std::cout << "SERVER_PORT" << "=" << this->vars["SERVER_PORT"] << std::endl;
    // std::cout << "QUERY_STRING" << "=" << this->vars["QUERY_STRING"] << std::endl;
}

char **Cgi::getEnv()
{
    int size = this->vars.size();
    char **env = new char *[size + 1];
    std::map<std::string, std::string>::iterator it = this->vars.begin();
    int i = 0;
    while (it != this->vars.end())
    {
        env[i] = NULL;
        std::string value = it->first + "=" + it->second;
        env[i] = new char[value.size() + 1];
        std::strcpy(env[i], value.c_str());
        i++;
        it++;
    }
    env[size] = NULL;
    return env;
}

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

std::string Cgi::executeScript(char **command)
{
    char **env = this->getEnv();
    std::string output = "error";
    int pipes[2];
    if (pipe(pipes) < 0)
    {
        std::cerr << "could not pipe" << std::endl;
        exit(1);
    }
    // if (req.getMethod() == "POST")
    //     write(pipes[1], this->req.getBody().c_str(), this->req.getBody().size());
    int pid = fork();
    if (pid < 0)
    {
        std::cerr << "problem forking" << std::endl;
        exit(1);
    }
    else if (pid == 0)
    {
        // std::cout << "child reading body: " << readfromFd(pipes[0]) << std::endl;
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
        execve(command[0], (char * const *)command, env);       
    }
    else
    {
        waitpid(pid, NULL, 0);
        close(pipes[1]);
        output = readfromFd(pipes[0]);
        // std::cout << output << std::endl;
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

