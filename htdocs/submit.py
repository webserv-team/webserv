import os

# Print all environment variables

print("<h1>Environment Variables</h1>")
print("<ul>")

for key, value in os.environ.items():
    print(f'<li>{key}: {value}</li>')

print("</ul>")
