import cgi
import user
import os
import sys
import cookie
# import cgitb
# cgitb.enable()
# string = ""

headers = []

def print_headers(headers: list[str]) -> None:
	# print("Status: 200 OK")
	# print("Content-type: text/html")
	for h in headers:
		print(h)
	print()


def verifyLogin(username: str, password: str) -> str:
	global headers
	logger = user.User("", username, password, "")
	string: str = ""

	if logger.getUser(username, password):
		envcookie = os.getenv("cookie")
		if not envcookie:
			SessionId = cookie.generateSessionId()
			newCookie = cookie.generateCookie(SessionId)
			headers.append(newCookie)
			logger.updateSessionId(SessionId)
		string += "<h1> Welcome " + username + " and your new session-id is " + logger.sessionID + "</h1>"
		string += "<p><a href='/'>Go back</a></p>"

	elif logger.getUserByUsername(username):
		string += "<h1>Incorrect password</h1>"

	else:
		string += "<h1> User not found! </h1>\n"
		string += "<p><a href='../src/register.html'>Register</a></p>"
	# print("printing headers1-----------------------------------------------------", file=sys.stderr)
	# print_headers(headers)
	return string

def tryCookie() -> None:
	cookies = os.getenv("Cookie")
	if cookies:
		try:
			_, session_id = cookies.split("=")
		except:
			c, _ = cookies.split(";")
			_, session_id = cookies.split("=")
		print("cookie found", file=sys.stderr)
		session_id = session_id.rstrip('\r\n')
		logger = user.User(session_id, "default", "default", "default")

		if logger.getUserByID(session_id):
			print()
			print("<html><body>")
			print("<h1> Login Program </h1>")
			print("<h1>no need to sign in user: " + logger.username + "</h1>")
			print("<p><a href='/'>Go back</a></p>")
			print("</body></html>")
			print("print done", file=sys.stderr)
			exit(0)

# uses querey string from env
def handle_get() -> str:
	string: str = ""
	try:
		form = cgi.FieldStorage()
		if "username" in form and "password" in form:
			string = verifyLogin(form["username"].value, form["password"].value)
		else:
			tryCookie()

	except TypeError:
		print_headers(headers)
		print("<html><body>")
		print("<h1> Login Program </h1>")
		print("<h1>please enter user info</h1>")
		print("<form method='post' action='login.py'>")
		print("<label>Username : </label> ")
		print("<input type='text' placeholder='Enter Username' name='username' required>")
		print("<label>Password : </label> ")
		print("<input type='password' placeholder='Enter Password' name='password' required>")
		print("<input type='submit' value='Submit'/>")
		print("</form>")
		print("</body></html>")
		exit(0)
	print_headers(headers)
	return string


# uses the stdin
def handle_post() -> str:
	qString = input()
	string: str = ""
	try:
		username, password = qString.split("&")
		_, username = username.split("=")
		_, password = password.split("=")
		string = verifyLogin(username, password)
	except:
		string = "<h1>invalid string</h1>"
	print_headers(headers)
	return string


if __name__ == '__main__':
	string: str

	if os.getenv("REQUEST_METHOD") == "GET":
		string = handle_get()
	elif os.getenv("Method") == "POST":
		string = handle_post()
	else:
		string = "METHOD NOT ALLOWED"
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
