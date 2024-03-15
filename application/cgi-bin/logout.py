import cgi
import user
import os
import sys
import cookie
# import cgitb
# cgitb.enable()
string = ""

cookies = os.getenv("Cookie")
if cookies:
	_, session_id = cookies.split("=")
	session_id = session_id.rstrip('\r\n')
	logger = user.User(session_id, "default", "default", "default")

	if logger.getUserByID(session_id):
		print(f"Set-Cookie: session-id=; Domain=localhost; expires=Thu, 01 Jan 1970 00:00:00 GMT; Path=/")
		print()
		print("<html><body>")
		print("<h1> Logout Program </h1>")
		print("<h2>logout successful")
		print("<p><a href='/'>Go back</a></p>")
		print("</body></html>")
		exit(0)

print()
print("<html><body>")
print("<h1> Logout Program </h1>")
print("<h2>you are not logged in")
print("</form>")
print("</body></html>")
