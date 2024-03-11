import sqlite3

class User:
	def __init__(self, userName="", password="", firstName=""):
		self.userName = userName
		self.password = password
		self.firstName = firstName
	
		self.connection = sqlite3.connect("database.db")
		self.cursor = self.connection.cursor()

		self.cursor.execute("""
		CREATE TABLE IF NOT EXISTS users (
					userName TEXT,
					password TEXT,
					firstName TEXT
		)
		""")

	def getUserByID(self, sessionID):
		self.cursor.execute("""
		SELECT * FROM users
		WHERE sessionID = ?
		""", (sessionID,))

		results = self.cursor.fetchone()
		if results:
			self.username = results[0]
			self.password = results[1]
			self.firstname = results[2]
			return self
			
		else:
			return None
			print("User not found.")
	def getUserByUsername(self, userName=""):
		self.cursor.execute("""
		SELECT * FROM users
		WHERE userName = ?
		""", (userName,))

		results = self.cursor.fetchone()
		if results:
			self.username = results[0]
			self.password = results[1]
			self.firstname = results[2]
			return True
		else:
			# print("User not found.")
			return False
		
	def getUser(self, userName="", password=""):
		self.cursor.execute("""
		SELECT * FROM users
		WHERE userName = ? AND password = ?
		""", (userName, password))

		results = self.cursor.fetchone()
		if results:
			return True
		else:
			return False

	def addUser(self):
		self.cursor.execute("""
			INSERT INTO users VALUES
			('{}', '{}', '{}')		  
			""".format(self.userName, self.password, self.firstName))
		
		self.connection.commit()
	# def updateSessionId(self, sessionID=""):
	# 	self.sessionID = sessionID
	# 	return ;
		# self.connection.close()

# p1 = User(123445, "mehdi1", "pss",  "medhi")
# p1.addUser()

# p2 = User(1234452, "mehdi123", "pssads",  "medhisas")
# p2.addUser()

# p2 = User(12, "mehdi123asdfa", "pssadfgfadsads",  "mefsdagfdhisas")
# p2.addUser()

# cursor.execute("SELECT * FROM users")
# results = cursor.fetchall()
# print(results)
# connection.close()
		