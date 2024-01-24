/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/24 12:13:45 by hoigag            #+#    #+#             */
/*   Updated: 2024/01/24 15:27:55 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <unistd.h>

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
std::string executeScript(char **command)
{
    std::string output = "error";
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
        execve(command[0], (char * const *)command, NULL);       
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




#include <string>
using namespace std;
int main()
{
    const char *commnad[3] = {"/Users/hoigag/.brew/bin/python3", "file.py", "NULL"};
    std::cout << executeScript((char ** )commnad);
}