#!/usr/bin/env python3
import os
import cgi
import warnings

# Suppress the deprecation warning for the 'cgi' module
warnings.filterwarnings("ignore", category=DeprecationWarning, module="cgi")
# X-Powered-By: PHP/7.4.10
# Content-type: text/html; charset=UTF-8


# Get the content length from the environment or set it to 0
content_length = int(os.environ.get("CONTENT_LENGTH", 0))

# Print HTTP headers
content = """
<!DOCTYPE html>
<html>
<head>
<title>Login Page</title>
<link rel="stylesheet" type="text/css" href="../styles/style.css">
</head>
<body>
""";
# Print the HTML page


# while True:
#     pass
# Check if the request method is POST
form = cgi.FieldStorage() 
if "REQUEST_METHOD" in os.environ and os.environ["REQUEST_METHOD"] == "POST":
    # Parse the POST data using the cgi module

    # Retrieve the value of the 'username' and 'password' fields from the form
    if "username" in form and "password" in form:
        username = form.getvalue("username")
        password = form.getvalue("password")

        # Display a welcome message
        content += '<div class="welcome-container">'
        content += f"<h2>Welcome, {username}!</h2>"
        content += f"<p>Your password is {password}.</p>"
        content += '</div>'
    else:
        # Display an error message
        content += '<div class="error-container">'
        content += "<p>Error: Username or password not provided.</p>"
        content += '</div>'
else:
    # Display an error message
    content += "<div class=\"login-form-container\">"
    content += "<h2>This is a get request</h2>"
    if "username" in form and "password" in form:
        username = form.getvalue("username")
        password = form.getvalue("password")
        content += f"<p>username : {username} </p>"
        content += f"<p>password : {password} </p>"
    # Display the login form
    content += '<a href="form.html">LOGIN</a>'
    content += "</div>"

content += "</body>"
content += "</html>"
# Print the HTML content

headers = f"""
Content-Type: text/html\r\n
Content-Length: {len(content)}\r\n
Status: 200 OK"""

print(headers, end="\r\n\r\n")
print(content)
