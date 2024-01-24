#!/usr/bin/env python3
import os

# Print the Content-Type header
print("Content-Type: text/html\n")

# Access CGI environment variables
print("<html>")
print("<head><title>CGI Environment Variables</title></head>")
print("<body>")
print("<h1>CGI Environment Variables</h1>")

print("<ul>")
for key, value in os.environ.items():
    print("<li><strong>{}</strong>: {}</li>".format(key, value))
print("</ul>")

print("</body>")
print("</html>")
