# from login import User

body = input()

username, password, firstname = body.split('&')
username = username.removeprefix("username=")
password = password.removeprefix("password=")
firstname = firstname.removeprefix("firstname=")


print("<!DOCTYPE html>")
print("<html>")
print("<head>")
print("<title>Form Submission Result</title>")
print("<style>h1{ text-align: center; }pre{ text-align: center; }</style>")
print("</head>")
print("<body>")
print("<h1>Form Submission Result</h1>")
print("<pre>welcome: " + firstname + "</pre>")
print("</body>")
print("</html>")
