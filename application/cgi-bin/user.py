import sqlite3
import sys
class User:
	def __init__(self, sessionID: str, userName: str, password: str, firstName: str):
		self.sessionID = sessionID
		self.userName = userName
		self.password = password
		self.firstname = firstName

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

	def getUserByID(self, id: str):
		self.cursor.execute("""
		SELECT * FROM users
		WHERE sessionID = ?
		""", (id,))
		results = self.cursor.fetchone()
		print(results[0], file=sys.stderr)
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
			""".format(self.sessionID, self.userName, self.password, self.firstname))

		self.connection.commit()

	def updateSessionId(self, sessionID: str):
		try:
			sqliteConnection = sqlite3.connect('database.db')
			cursor = sqliteConnection.cursor()
			sql_update_query = """Update users set sessionID = ? where userName = ?"""
			data = (sessionID, self.userName)
			cursor.execute(sql_update_query, data)
			sqliteConnection.commit()
			print("Record Updated successfully", file=sys.stderr)
			cursor.close()

		except sqlite3.Error as error:
			print("Failed to update sqlite table", error)

	def printuser(self):
		print(f"Session ID: {self.sessionID}, Username: {self.userName}, Password: {self.password}, First Name: {self.firstname}", file=sys.stderr)

	def printAllUsers(self):
		try:
			self.cursor.execute("SELECT * FROM users")
			all_users = self.cursor.fetchall()
			for user in all_users:
				sessionID, userName, password, firstName = user
				print(f"Session ID: {sessionID}, Username: {userName}, Password: {password}, First Name: {firstName}\n", file=sys.stderr)
		except sqlite3.Error as error:
			print("Failed to fetch users from the database:", error)




# user_instance = User("4be7305f-8d7a-as-9886-72c23775ba32", "a", "a", "a")

# # Call the getUserByID method with the sessionID you want to search for
# id = "4be7305f-8d7a-48fe-9886-72c23775ba32"
# user_instance.getUserByID(id)
# user_instance.tester(id, "a")



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

