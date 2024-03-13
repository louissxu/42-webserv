import cgi
import user
import os
import sys
import cookie

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
		string += "<h1> Welcome " + username + ", your password is " + password + " and new session-id is " + logger.sessionID + "</h1>"
	elif logger.getUserByUsername(username):
		string += "<h1>Incorrect password</h1>"
	else:
		string += "<h1> User not found! </h1>\n"
		string += "<p><a href='../register/index.html'>Register</a></p>"


form = cgi.FieldStorage()
if "username" in form and "password" in form:
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
