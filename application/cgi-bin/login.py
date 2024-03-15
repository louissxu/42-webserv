<<<<<<< HEAD
import os
import user

try:
	body = input()
except EOFError:
	print("Login.py read error\n")
	exit()

if (body == ""):
	exit()

username, password = body.split('&')
username = username.removeprefix("username=")
password = password.removeprefix("password=")

# session_id = os.getenv("session_id")
# if (session_id == None):
# 	session_id = 12345

oldUser = user.User(0, username, password, "")
if (oldUser.getUserByUsername(username) == True):
	print("<!DOCTYPE html>")
	print("<html>")
	print("<head>")
	print("<title>Form Submission Result</title>")
	print("<style>h1{ text-align: center; }pre{ text-align: center; }</style>")
	print("</head>")
	print("<body>")
	print("<h1>Form Submission Result</h1>")
	print("<pre>welcome back: " + username + "</pre>")
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
	print("<pre>no such user found: " + username + "</pre>")
	print("</body>")
	print("</html>")




# session_id = os.getenv("session_id")
# newUser = user.User(session_id, username, password, firstname)

# print ("HTTP/1.1 200 OK")
# print ("Connection: Keep-Alive")
# print ("Content-Length: 221")
# print ("Content-Type: text/html")
# print ("Server: mehdi's_webserv")
# print

=======
import cgi
import user
import os
import sys
import cookie
# import cgitb
# cgitb.enable()
string = ""

def verifyLogin(form):
	global string
	username = form["username"].value
	password = form["password"].value
	logger = user.User("", username, password, "")
	if logger.getUser(username, password):
		print(f"\t\tlogger before = {logger.printuser()}", file=sys.stderr)
		envcookie = os.getenv("cookie")
		if not envcookie:
			newcookie = cookie.generateCookie()
			cookie.printCookie(newcookie)
			logger.updateSessionId(newcookie)
		string += "<h1> Welcome " + username + " and new session-id is " + logger.sessionID + "</h1>"
	elif logger.getUserByUsername(username):
		string += "<h1>Incorrect password</h1>"
	else:
		string += "<h1> User not found! </h1>\n"
		string += "<p><a href='../register/index.html'>Register</a></p>"


form = cgi.FieldStorage()
if "username" in form and "password" in form:
	# print(repr(os.getenv("QUERY_STRING")), file=sys.stderr)
	# print ("username = " + form["username"].value + "\n", file=sys.stderr)
	# print ("username = " + form["password"].value + "\n", file=sys.stderr)
	verifyLogin(form)
else:
	cookies = os.getenv("Cookie")
	if cookies:
		_, session_id = cookies.split("=")
		session_id = session_id.rstrip('\r\n')
		logger = user.User(session_id, "default", "default", "default")

		if logger.getUserByID(session_id):
			print()
			print("<html><body>")
			print("<h1> Login Program </h1>")
			print("<h1>no need to sign in user: " + logger.username + "</h1>")
			print("<p><a href='/'>Go back</a></p>")
			print("</body></html>")
			exit(0)

print()
print("<html><body>")
print("<h1> Login Program </h1>")
if string == "":
	print("<h1>please enter user info</h1>")
else:
	print(string)
print("<form method='post' action='login.py'>")
print("<label>Username : </label> ")
print("<input type='text' placeholder='Enter Username' name='username' required>")
print("<label>Password : </label> ")
print("<input type='password' placeholder='Enter Password' name='password' required>")
print("<input type='submit' value='Submit'/>")
print("</form>")
print("</body></html>")
>>>>>>> origin/main
