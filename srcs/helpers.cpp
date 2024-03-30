/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hoigag <hoigag@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/27 10:37:31 by hoigag            #+#    #+#             */
/*   Updated: 2024/03/30 22:08:16 by hoigag           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "helpers.hpp"

void setSocketToBeReusable(int sock)
{
	int opt = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		std::string err = "Setsockopt error: " + itoa(sock);
		throw std::runtime_error(err);
	}
}

void setSocketToNonBlocking(int socket)
{
	int status = fcntl(socket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	if (status < 0)
	{
		std::string err = "could not set socket " + itoa(socket) + " to non blocking";
		throw std::runtime_error(err);
	}
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
		throw std::runtime_error("could not open file load file function");
	std::ostringstream buffer;
	buffer << inFile.rdbuf();
	inFile.close();
	return buffer.str();
}

bool isFileExists(const std::string& path)
{
	return access(path.c_str(), F_OK) == 0;
}

std::string getContentTypeFromCgiOutput(std::string& content)
{
		
	if (content.empty())
		return "";
	size_t pos = content.find("Content-Type: ");
	if (pos == std::string::npos)
		return "";
	std::string contentType = content.substr(pos + 14);
	pos = contentType.find(";");
	if (pos != std::string::npos)
		contentType = contentType.substr(0, pos);
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
		throw std::runtime_error("could not open directory");
	
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

bool uploadFiles(Request& req, Location& location)
{
	vector<s_tuple > data = req.getMultipart();
	std::string body = "";
	for (size_t i = 0; i < data.size(); i++)
	{
		if (data[i].fileName.empty())
			body += data[i].name + "=" + data[i].value + "&";
		else
			{
				std::string uploadDir = location.root + location.uploadPath;
				if (!isFileExists(uploadDir))
				{
					if (mkdir(uploadDir.c_str(), 0777) < 0)
					{
						std::cerr << "could not create directory " << uploadDir << std::endl;
						return false;
					}
				}
				std::string uploadPath = uploadDir + "/" + data[i].fileName;
				std::ofstream outfile(uploadPath);
				if (!outfile.is_open())
					return false;
				if (data[i].value.empty())
					std::cerr << "the file content is empty" << std::endl;
				else
				{
					outfile.write(data[i].value.data(), data[i].value.size());
					outfile.close();
				}
			}
	}
	return true;
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

string itoa(long long n) 
{
	string s;
	if (n == 0)
		return "0";
	while (n) {
		s += n % 10 + '0';
		n /= 10;
	}
	reverse(s.begin(), s.end());
	return s;
}

bool isAllowdCgiExtension(std::string path)
{
	std::string extension = path.substr(path.find_last_of(".") + 1);
	if (extension == "py")
		return true;
	return false;
}