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
echo "  <pre>welcome: $username</pre>"
echo "</body>"
echo "</html>"

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
