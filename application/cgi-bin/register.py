import os
import user
<<<<<<< HEAD
import sqlite3

try:
	body = input()
except EOFError:
	print("hello")
	exit()

if (body == ""):
	exit()

username, password, firstname = body.split('&')
username = username.removeprefix("username=")
password = password.removeprefix("password=")
firstname = firstname.removeprefix("firstname=")

session_id = os.getenv("session_id")
if (session_id == None):
	session_id = 12345
# print (session_id)
newUser = user.User(session_id, username, password, firstname)

if (newUser.getUserByUsername(username) == False):
	newUser.addUser()
	print("<!DOCTYPE html>")
	print("<html>")
	print("<head>")
	print("<title>Form Submission Result</title>")
	print("<style>h1{ text-align: center; }pre{ text-align: center; }</style>")
	print("</head>")
	print("<body>")
	print("<h1>Form Submission Result</h1>")
	print("<pre>thanks for registering: " + firstname + "</pre>")
	print("</body>")
	print("</html>")
else:
	print("<!DOCTYPE html>")
	print("<html>")
	print("<head>")
	print("<title>Form Submission Result</title>")
	print("<style>h1{ text-align: center; }pre{ text-align: center; }</style>")
	print("</head>")
	print("<body>")
	print("<h1>Form Submission Result</h1>")
	print("<pre>welcome back you are already a member: " + firstname + "</pre>")
	print("</body>")
	print("</html>")



# print ("HTTP/1.1 200 OK")
# print ("Connection: Keep-Alive")
# print ("Content-Length: 221")
# print ("Content-Type: text/html")
# print ("Server: mehdi's_webserv")
# print 

=======
import cgi

print()
print("<!DOCTYPE html>")
print("<html>")
print("<head>")
print("<title>Form Submission Result</title>")
print("<style>h1{ text-align: center; }pre{ text-align: center; }</style>")
print("</head>")
print("<body>")


form = cgi.FieldStorage()
if "username" in form and "password" in form:
	username = form["username"].value
	password = form["password"].value
	firstname = form["firstname"].value

	print("<h1>Form Submission Result</h1>")
	newUser = user.User("", username, password, firstname)

	if (newUser.getUserByUsername(username) == False):
		newUser.addUser()
		print("<pre>thanks for registering: " + firstname + "</pre>")
		print("<p><a href='login.py'>login</a></p>")
	else:
		print("<pre>member already exists: " + firstname + " " + newUser.sessionID + "</pre>")
		print("<p><a href='login.py'>login</a></p>")

print("</body>")
print("</html>")
>>>>>>> origin/main
