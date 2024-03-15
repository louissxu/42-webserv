from http import cookies
import datetime, uuid
ck = cookies.SimpleCookie()

def generateCookie():
	return str(uuid.uuid4())

def printCookie(id: str):
	ck['session-id'] = id
	ck['session-id']['domain'] = 'localhost'
	ck['session-id']['path'] = '/'
	expires = datetime.datetime.utcnow() + datetime.timedelta(days=1) # expires in 30 days
	ck['session-id']['expires'] = expires.strftime("%a, %d %b %Y %H:%M:%S GMT")

	print (ck.output())
# >>> print ck.output()
# Set-Cookie: session=9249169b-4c65-4daf-8e64-e46333aa5577; Domain=foo.com; expires=Mon, 01 Aug 2011 07:51:53 GMT; Path=/