#!/usr/bin/env python
import cgi
import os
from io import BytesIO as IO
import base64

def html_print(key, val):
    print(f"<!-- key: {key}    val: {val}-->")

# Set target directory for uploads
application_directory = "application"
save_folder_name = "uploaded-files"

# Create HTML header
print("Content-type: text/html\n")
print("<html><body>")
print("<h1> File Upload </h1>")

print(f"<!-- {os.getenv('Content-Type')}-->")

html_print("start of query string is: ", os.getenv("QUERY_STRING")[:10])
if (os.getenv("Content-Type")[0:9] == "multipart"):
    body = base64.b64decode(os.getenv("QUERY_STRING").encode())
else:
    body = os.getenv("QUERY_STRING").encode()

html_print("start of body is: ", body[:10])

# Check if form was submitted
form = cgi.FieldStorage(
    IO(body),
    headers={"content-type": os.getenv("Content-Type"), "content-length": os.getenv("Content-Length")},
    environ={"REQUEST_METHOD": os.getenv("Method")}
)

item = form.value[0]
print(f"<!-- name is: {item.name}, filename is: {item.filename}, value is: {item.value[:10]} -->")

field_name = item.name
file_name = item.filename
binary_data = item.value

html_print("type of binary_data", type(binary_data))

current_pwd = os.getcwd()
print(f"<!- current pwd is {current_pwd}-->")
upload_directory = os.path.join(os.getcwd(), application_directory, save_folder_name)
if not os.path.exists(upload_directory):
    os.makedirs(upload_directory)

file_path = os.path.join(upload_directory, os.path.basename(file_name)) 
print(f"<!-- file path is {file_path}-->")

if os.path.exists(file_path):
    print("<p>Sorry, file already exists.</p>")
else:
        # Try to upload file
    try:
        with open(file_path, "wb") as f:
            f.write(binary_data)
        print(f"<p>The file {file_name} has been uploaded.</p>")
    except IOError as e:
        print(f"<!-- {e.errno} -->")
        print(f"<!-- {e} -->")
        print(f"<p>Sorry there was an error uploading your file</p>")

# Close HTML body and footer
print("</body></html>")
