import cgi
import user
import os
import sys
import cookie
import pickle

string = ""

def verifyLogin(form):
	global string
	username = form["username"].value
	password = form["password"].value
	logger = user.User("", "", "", "")
	if logger.getUser(username, password):
		session_id = os.getenv("cookie")
		if not session_id:
			session_id = cookie.generateCookie()
			cookie.printCookie(session_id)
			print(f"\t\tlogger before = {logger}", file=sys.stderr)
			logger.updateSessionId(session_id)
			print(f"\t\tlogger after = {logger}", file=sys.stderr)
			# with open(session_id, "wb") as sessionfile:
			# 	pickle.dump(logger, sessionfile)
		string += "<h1> Welcome " + username + ", your password is " + password + " " + logger.sessionID + "</h1>"
	elif logger.getUserByUsername(username):
		string += "<h1>Incorrect password</h1>"
	else:
		string += "<h1> User not found! </h1>\n"
		string += "<p><a href='../register/index.html'>Register</a></p>"


form = cgi.FieldStorage()
if "username" in form and "password" in form:
	verifyLogin(form)
else:
	# print(os.environ)
	cookies = os.getenv("Cookie")
	print(f"\t\tcookie = {cookies}", file=sys.stderr)
	if cookies:
		# print(session_id, file=sys.stderr)
		_, session_id = cookies.split("=")
		print(f"\t\tsession id = {session_id}", file=sys.stderr)
		# print("\n\n" + session_id + "\n\n")
		# session_id, _ = session_id.split(";")
		logger = user.User("", "", "", "")
		if logger.getUserByID(session_id):
			print()
			print("<html><body>")
			print("<h1> Login Program </h1>")
			print("<h1>no need to sign in user: " + logger.username + "</h1>")
			print("</body></html>")
			exit(0)
		else:
			print(f"\t\tcould not find user", file=sys.stderr)
		# with open(session_id, "rb") as sessionfile:
		# 	data = pickle.load(sessionfile)
		# 	print(data)

print()
print("<html><body>")
print("<h1> Login Program </h1>")
# print(string)
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
