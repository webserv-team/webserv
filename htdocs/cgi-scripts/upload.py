#!/usr/bin/env python3
import os
import cgi

def printHeader():
    print("Content-Type: text/html\r\n\r\n", end="")

# Set the upload directory
upload_dir = '/Users/hoigag/cursus/webserv/upload'

# Create an instance of the FieldStorage class to parse the form data
form = cgi.FieldStorage()

# Check if the file field is in the form data
if 'file' in form:
    # If the form field is a list, iterate over each file
    if isinstance(form['file'], list):
        printHeader()
        for file_item in form['file']:
            # Check if the file was successfully uploaded
            if file_item.file and file_item.filename:
                # Get the filename
                filename = os.path.basename(file_item.filename)

                # Set the target path for saving the file
                target_path = os.path.join(upload_dir, filename)

                # Save the file to the upload directory
                with open(target_path, 'wb') as f:
                    f.write(file_item.file.read())

                # Print a success message for each file
                print(f"<p style='color: green;'>File {filename} uploaded successfully!</p>")
            else:
                # Print an error message if file upload failed
                print("<p style='color: red;'>Could not upload one or more files!</p>")
    else:
        # Handle the case when only one file is uploaded
        printHeader()
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
            print(f"<h1>File Upload</h1>")
            print(f"<p style='color: green;'>File {filename} uploaded successfully!</p>")
        else:
            # Print an error message if file upload failed
            print("<p style='color: red;'>Could not upload the file!</p>")
else:
    # Print an error message if no file was uploaded
    printHeader()
    print("<h1>File Upload</h1>")
    print("<p style='color: red;'>Error: No file uploaded.</p>")
