import cgi
import uuid
import user
import os
import sys

def verifyLogin(form):
	username = form["username"].value
	password = form["password"].value
	logger = user.User("", "", "")
	if (logger.getUser(username, password) == True):
		# Generate a random UUID (session ID)
		# session_id = os.getenv("session_id")
		# if not session_id:
		# 	session_id = uuid.uuid4()
		# 	logger.updateSessionId(str(session_id))
		# 	print("session_id " + str(logger.sessionID), file=sys.stderr)
			# print("Set-Cookie: session-id=" + str(session_id) + )
		
		# print("Generated session ID:", session_id)
		print("<h1> Welcome " + username + ", your password is " + password + "</h1>")
	else:
		print("<h1> User not found! </h1>")
		print("<p><a href='../register/index.html'>Register</a></p>")
	

print("<html><body>")
print("<h1> Login Program </h1>")

session_id = os.getenv("session_id")
if (session_id):
	print("session_id" + session_id, file=sys.stderr)
	logger = user.User.getUserByID(session_id)
	if logger != None:
		print("<h1> Welcome " + logger.username + "</h1>")
	# logger.updateSessionId(session_id)
else:
	form = cgi.FieldStorage()
	if "username" in form and "password" in form:
		verifyLogin(form)
    # username = form["username"].value
    # password = form["password"].value
    # print("<h1> Welcome " + username + ", your password is " + password + "</h1>")

print("<form method='post' action='login.py'>")
print("<label>Username : </label> ")
print("<input type='text' placeholder='Enter Username' name='username' required>")
print("<label>Password : </label> ")
print("<input type='password' placeholder='Enter Password' name='password' required>")
print("<input type='submit' value='Submit'/>")
print("</form>")
print("</body></html>")










# import cgi

# import cgitb
# cgitb.enable()

# print("<html><body>")
# print("<h1> Login Program </h1>")

# form = cgi.FieldStorage()
# if "username" and "password" in form:
#     name = form["name"].value
#     print("<h1> welcome " + name + "</h1>")
#     password = form["password"].value
#     print("<h1> welcome " + name + "password = " + password + "</h1>")

# print("<form method='post' action='login.py'>")  # Fixed typo in action attribute

# print("<label>Username : </label> ")
# print("<input type='text' placeholder='Enter Username' username='username' required>")
# print("<label>Password : </label> ")
# print("<input type='password' placeholder='Enter Password' passwordname='password' required>")
# # print("<button type="submit'>Login</button>")
# # print("<p>User Name: <input type='text' username='name'/></p>")
# # print("<p>Password Name: <input type='text' passwordname='name'/></p>")
# print("<input type='submit' value='Submit'/>")  # Added submit button
# print("</form>")  # Added closing form tag
# print("</body></html>")
# import os
# import user

# try:
# 	body = input()
# except EOFError:
# 	print("Login.py read error\n")
# 	exit()

# if (body == ""):
# 	exit()

# username, password = body.split('&')
# username = username.removeprefix("username=")
# password = password.removeprefix("password=")
# session_id = os.getenv("session-id")
# if (session_id == None):
# 	session_id = 12345
# # session_id = os.getenv("session_id")
# # if (session_id == None):
# # 	session_id = 12345

# print("<!DOCTYPE html>")
# print("<html>")
# print("<head>")
# print("<title>Form Submission Result</title>")
# print("<style>h1{ text-align: center; }pre{ text-align: center; }</style>")
# print("</head>")
# print("<body>")
# print("<h1>Form Submission Result</h1>")

# oldUser = user.User(0, username, password, "")
# if (oldUser.getUserByUsername(username) == True):
# 	print("<pre>welcome back: " + username + "</pre>")
# else:
# 	print("<pre>no such user found: " + username + "</pre>")

# print("</body>")
# print("</html>")