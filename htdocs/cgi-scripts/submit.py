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
print("X-Powered-By: Python/3.8.5")
print("Content-Type: text/html; charset=UTF-8")
print(f"Content-Length: ")
print("Connection: close")
print("cache-control: no-cache")
print("\n\r\n\r")

# Print the HTML page
print("<html>")
print("<head>")
print("<title>Login Page</title>")
print('<link rel="stylesheet" type="text/css" href="../styles/style.css">')
print("</head>")
print("<body>")

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
        print('<div class="welcome-container">')
        print(f"<h2>Welcome, {username}!</h2>")
        print(f"<p>Your password is {password}.</p>")
        print('</div>')
    else:
        # Display an error message
        print('<div class="error-container">')
        print("<p>Error: Username or password not provided.</p>")
        print('</div>')
else:
    print('<div class="login-form-container">')
    print("<h2>This is a get request</h2>")
    if "username" in form and "password" in form:
        username = form.getvalue("username")
        password = form.getvalue("password")
        print(f"<p>username : {username} </p>")
        print(f"<p>password : {password} </p>")
    # Display the login form
    print('<a href="form.html">LOGIN</a>')
    print('</div>')

print("</body>")
print("</html>")


