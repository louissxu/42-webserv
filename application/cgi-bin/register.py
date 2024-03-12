import os
import user
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
