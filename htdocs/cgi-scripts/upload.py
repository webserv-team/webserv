#!/usr/bin/env python3
import cgi
import os

# Set the upload directory
upload_dir = '/home/hassan/cursus/webserv/upload'

# Create an instance of the FieldStorage class to parse the form data
form = cgi.FieldStorage()

# Check if the file field is in the form data
if 'file' in form:
    file_item = form['file']

    # Check if the file was successfully uploaded
    if file_item.file and file_item.filename:
        # Get the filename
        filename = os.path.basename(file_item.filename)

        # Set the target path for saving the file
        target_path = os.path.join(upload_dir, filename)

        # Save the file to the upload directory
        with open(target_path, 'wb') as f:
            f.write(file_item.file.read())

        # Print a success message
        print("<h1>File Upload</h1>")
        print(f"<p style='color: green;'>File {filename} uploaded successfully!</p>")
        # print(f"File {filename} uploaded successfully!")
    else:
        # Print an error message
        print("<p style='color: read;'>could not upload the file!</p>")
        # print("Error: File upload failed.")
else:
    # Print an error message if no file was uploaded
    print("Content-Type: text/html")
    print()
    print("Error: No file uploaded.")