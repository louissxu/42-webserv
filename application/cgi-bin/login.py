import os
import user

try:
	body = input()
except EOFError:
	print("hello")
	exit()

if (body == ""):
	exit()

username, password = body.split('&')
username = username.removeprefix("username=")
password = password.removeprefix("password=")

session_id = os.getenv("session_id")
# newUser = user.User(session_id, username, password, firstname)

# print ("HTTP/1.1 200 OK")
# print ("Connection: Keep-Alive")
# print ("Content-Length: 221")
# print ("Content-Type: text/html")
# print ("Server: mehdi's_webserv")
# print 

print("<!DOCTYPE html>")
print("<html>")
print("<head>")
print("<title>Form Submission Result</title>")
print("<style>h1{ text-align: center; }pre{ text-align: center; }</style>")
print("</head>")
print("<body>")
print("<h1>Form Submission Result</h1>")
print("<pre>welcome: " + username + "</pre>")
print("</body>")
print("</html>")
