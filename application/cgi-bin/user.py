import sqlite3

connection = sqlite3.connect("database.db")
cursor = connection.cursor()

cursor.execute("""
CREATE TABLE IF NOT EXISTS users (
			   sessionID INTEGER,
			   userName TEXT,
			   password TEXT,
			   firstName TEXT
)
""")

class User:
	def __init__(self, sessionID, userName="", password="", firstName=""):
		self.sessionID = sessionID
		self.userName = userName
		self.password = password
		self.firstName = firstName
	
		self.connection = sqlite3.connect("database.db")
		self.cursor = connection.cursor()

	def getUser(self, sessionID):
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
		else:
			print("User not found.")

	def addUser(self):
		self.cursor.execute("""
			INSERT INTO users VALUES
			({}, '{}', '{}', '{}')		  
			""".format(self.sessionID, self.userName, self.password, self.firstName))
		
		self.connection.commit()
		self.connection.close()

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
		