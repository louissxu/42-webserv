#!/usr/bin/env python
import cgi
import os

# Set target directory for uploads
target_dir = "uploads/"

# Create HTML header
print("Content-type: text/html\n")
print("<html><body>")
print("<h1> Login Program </h1>")

# Check if form was submitted
form = cgi.FieldStorage()
if "submit" in form:
    # Get file details
    file_item = form["fileToUpload"]
    target_file = os.path.join(target_dir, os.path.basename(file_item.filename))
    upload_ok = 1
    image_file_type = os.path.splitext(target_file)[1].lower()

    # Check if image file is an actual image
    if file_item.file:
        check = file_item.file.read()
        if check.startswith(b'\xFF\xD8') and check.endswith(b'\xFF\xD9'):
            print("<p>File is an image.</p>")
        else:
            print("<p>File is not an image.</p>")
            upload_ok = 0

    # Check if file already exists
    if os.path.exists(target_file):
        print("<p>Sorry, file already exists.</p>")
        upload_ok = 0

    # Check file size
    if file_item.file and len(check) > 500000:
        print("<p>Sorry, your file is too large.</p>")
        upload_ok = 0

    # Allow certain file formats
    allowed_formats = [".jpg", ".png", ".jpeg", ".gif"]
    if image_file_type not in allowed_formats:
        print("<p>Sorry, only JPG, JPEG, PNG & GIF files are allowed.</p>")
        upload_ok = 0

    # Check if upload_ok is set to 0 by an error
    if upload_ok == 0:
        print("<p>Sorry, your file was not uploaded.</p>")
    else:
        # Try to upload file
        try:
            with open(target_file, "wb") as f:
                f.write(file_item.file.read())
            print("The file {} has been uploaded.".format(cgi.escape(file_item.filename)))
        except IOError:
            print("<p>Sorry, there was an error uploading your file.</p>")

# Close HTML body and footer
print("</body></html>")
