import sqlite3

class User:
	def __init__(self, sessionID: str, userName: str, password: str, firstName: str):
		self.sessionID = sessionID
		self.userName = userName
		self.password = password
		self.firstName = firstName

		self.connection = sqlite3.connect("database.db")
		self.cursor = self.connection.cursor()

		self.cursor.execute("""
		CREATE TABLE IF NOT EXISTS users (
					sessionID TEXT,
					userName TEXT,
					password TEXT,
					firstName TEXT
		)
		""")

	def getUserByID(self, sessionID: str):
		self.cursor.execute("""
		SELECT * FROM users
		WHERE sessionID = ?
		""", (sessionID,))

		results = self.cursor.fetchone()
		if results:
			self.sessionID = results[0]
			self.username = results[1]
			self.password = results[2]
			self.firstname = results[3]
			return True

		else:
			return False

	def getUserByUsername(self, userName: str):
		self.cursor.execute("""
		SELECT * FROM users
		WHERE userName = ?
		""", (userName,))

		results = self.cursor.fetchone()
		if results:
			self.sessionID = results[0]
			self.username = results[1]
			self.password = results[2]
			self.firstname = results[3]
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
			self.sessionID = results[0]
			self.username = results[1]
			self.password = results[2]
			self.firstname = results[3]
			return True
		else:
			return False

	def addUser(self):
		self.cursor.execute("""
			INSERT INTO users VALUES
			('{}', '{}', '{}', '{}')
			""".format(self.sessionID, self.userName, self.password, self.firstName))

		self.connection.commit()

	def updateSessionId(self, sessionID=""):
		try:
			sqliteConnection = sqlite3.connect('database.db')
			cursor = sqliteConnection.cursor()
			sql_update_query = """Update users set sessionID = ? where userName = ?"""
			data = (sessionID, self.userName)
			cursor.execute(sql_update_query, data)
			sqliteConnection.commit()
			# print("Record Updated successfully")
			cursor.close()

		except sqlite3.Error as error:
			print("Failed to update sqlite table", error)

	def printAllUsers(self):
		try:
			self.cursor.execute("SELECT * FROM users")
			all_users = self.cursor.fetchall()
			for user in all_users:
				sessionID, userName, password, firstName = user
				print(f"Session ID: {sessionID}, Username: {userName}, Password: {password}, First Name: {firstName}")
		except sqlite3.Error as error:
			print("Failed to fetch users from the database:", error)
		# finally:
		# 	if sqliteConnection:
		# 		sqliteConnection.close()
		# 		print("The SQLite connection is closed")
	# 	self.sessionID = sessionID
	# 	return ;
		# self.connection.close()

# p1 = User(123445, "mehdi1", "pss",  "medhi")
# p1.addUser()

# p2 = User(1234452, "mehdi123", "pssads",  "medhisas")
# p2.addUser()

# p2 = User(12, "mehdi123asdfa", "pssadfgfadsads",  "mefsdagfdhisas")
# p2.addUser()

# p = User("", "a", "a", "a")
# p.updateSessionId("4b73230a-e0c9-41f0-b740-98e075b08e32")
# connection = sqlite3.connect("../../database.db")
# cursor = connection.cursor()
# cursor.execute("SELECT * FROM users")
# results = cursor.fetchall()
# print(results)
# connection.close()

# p = User("", "", "", "")
# p.printAllUsers()
# p.getUserByID("4b73230a-e0c9-41f0-b740-98e075b08e32")
# print (p.sessionID)

# connection = sqlite3.connect("database.db")
# cursor = connection.cursor()
# cursor.execute("SELECT * FROM users")
# results = cursor.fetchall()
# print(results)
# connection.close()

