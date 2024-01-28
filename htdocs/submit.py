import os, sys
import cgitb
import cgi

cgitb.enable()
content_length = int(os.environ["CONTENT_LENGTH"])
post_data = sys.stdin.read(content_length)
# Print HTTP headers
print("Content-type: text/html")
print()

# Print the HTML page
print("<html>")
print("<head>")
print("<title>Python CGI Script - Handling POST Data</title>")
print("</head>")
print("<body>")

# Check if the request method is POST
if "REQUEST_METHOD" in os.environ and os.environ["REQUEST_METHOD"] == "POST":
    # Get the input stream and read the POST data

    # Parse the POST data using the cgi module
    form = cgi.FieldStorage(fp=sys.stdin, environ=os.environ)

    # Retrieve the value of the 'username' field from the form
    if "username" in form:
        username = form["username"].value
        print(f"<h2>Welcome, {username}!</h2>")
    else:
        print("<p>Error: Username not provided.</p>")
else:
    print("<p>Error: This script expects a POST request.</p>")

print("</body>")
print("</html>")
