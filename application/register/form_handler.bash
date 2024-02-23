#!/bin/bash

# Set Content-Type header to indicate the type of content being returned
echo "Content-Type: text/html"
echo ""

# Read form data from standard input
read -n $CONTENT_LENGTH form_data

# Parse form data (in this example, just print it back)
# echo "<html><head><title>Form Submission Result</title></head><body>"
# echo "<h1>Form Submission Result</h1>"
# echo "<p>Received form data:</p>"
# echo "<pre>$form_data</pre>"
# echo "</body></html>"

echo "<!DOCTYPE html>"
echo "<html>"
echo "<head>"
echo "  <title>Form Submission Result</title>"
echo "  <style>"
echo "    h1 { text-align: center; }"
echo "    pre { text-align: center; }"
echo "  </style>"
echo "</head>"
echo "<body>"
echo "  <h1>Form Submission Result</h1>"
echo "  <pre>welcome: </pre>"
echo "</body>"
echo "</html>"