#!/bin/bash

# Read form data
read form_data
username=$(echo "$form_data" | awk -F'&' '{split($1, a, "="); print a[2]}')
password=$(echo "$form_data" | awk -F'&' '{split($2, a, "="); print a[2]}')

# Set Content-Type header to indicate the type of content being returned
# echo "Content-Type: text/html"
# echo ""

# Echo HTML content
# echo "<!DOCTYPE html><html><head><style>h1 {text-align: center;} pre {text-align: center;}</style><title>Form Submission Result</title></head><body>"
# echo "<h1>Form Submission Result</h1>"
# echo "<pre>welcome: $username</pre>"
# echo "</body></html>"


# echo "HTTP/1.1 200 OK\r\n"
# echo "Connection: Keep-Alive\r\n"
# echo "Content-Length: 245\r\n"
# echo "Content-Type: text/html\r\n"
# echo "Server: mehdi's_webserv\r\n"
echo "<!DOCTYPE html><html><head><title>Form Submission Result</title><style>h1{ text-align: center; }pre{ text-align: center; }</style></head><body><h1>Form Submission Result</h1><pre>welcome: $username</pre></body></html>"

# Flush the output
echo -n ""


# #!/bin/bash

# # Read form data from standard input
# read form_data
# username=$(echo "$form_data" | awk -F'&' '{split($1, a, "="); print a[2]}')
# password=$(echo "$form_data" | awk -F'&' '{split($2, a, "="); print a[2]}')


# # Set Content-Type header to indicate the type of content being returned
# # echo "Content-Type: text/html"
# # Parse form data (in this example, just print it back)
# echo "<!DOCTYPE html>"
# echo "<html><head><title>Form Submission Result</title></head><body>"
# echo "<h1>Form Submission Result</h1>"
# # echo "<p>Received form data:</p>"
# echo "<pre>welcome: $username</pre>"
# echo "</body></html>"
