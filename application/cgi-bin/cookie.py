import uuid

# Generate a random UUID (session ID)
session_id = uuid.uuid4()

print("Generated session ID:", session_id)

# import Cookie
# C = Cookie.SimpleCookie()
# C["fig"] = "newton"
# C["sugar"] = "wafer"
# print C # generate HTTP headers
# Set-Cookie: fig=newton
# Set-Cookie: sugar=wafer
# print C.output() # same thing
# Set-Cookie: fig=newton
# Set-Cookie: sugar=wafer
# C = Cookie.SimpleCookie()
# C["rocky"] = "road"
# C["rocky"]["path"] = "/cookie"
# print C.output(header="Cookie:")
# Cookie: rocky=road; Path=/cookie
# print C.output(attrs=[], header="Cookie:")
# Cookie: rocky=road
# C = Cookie.SimpleCookie()
# C.load("chips=ahoy; vienna=finger") # load from a string (HTTP header)
# print C
# Set-Cookie: chips=ahoy
# Set-Cookie: vienna=finger
# C = Cookie.SimpleCookie()
# C.load('keebler="E=everybody; L=\\"Loves\\"; fudge=\\012;";')
# print C
# Set-Cookie: keebler="E=everybody; L=\"Loves\"; fudge=\012;"
# C = Cookie.SimpleCookie()
# C["oreo"] = "doublestuff"
# C["oreo"]["path"] = "/"
# print C
# Set-Cookie: oreo=doublestuff; Path=/
# C["twix"] = "none for you"
# C["twix"].value
# 'none for you'
# C = Cookie.SimpleCookie()
# C["number"] = 7 # equivalent to C["number"] = str(7)
# C["string"] = "seven"
# C["number"].value
# '7'
# C["string"].value
# 'seven'
# print C
# Set-Cookie: number=7
# Set-Cookie: string=seven
# # SerialCookie and SmartCookie are deprecated
# # using it can cause security loopholes in your code.
# C = Cookie.SerialCookie()
# C["number"] = 7
# C["string"] = "seven"
# C["number"].value
# 7
# C["string"].value
# 'seven'
# print C
# Set-Cookie: number="I7\012."
# Set-Cookie: string="S'seven'\012p1\012."
# C = Cookie.SmartCookie()
# C["number"] = 7
# C["string"] = "seven"
# C["number"].value
# 7
# C["string"].value
# 'seven'
# print C
# Set-Cookie: number="I7\012."
# Set-Cookie: string=seven