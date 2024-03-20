import os
import cgi
import cgitb
cgitb.enable()

form = cgi.FieldStorage()

last_value = None
if "data_to_send" in form:
  last_value = form["data_to_send"].value


print(f'''
<!DOCTYPE html>
<html>
  <head>
    <title>POST Test</title>
  </head>
  <body>
    <!-- {form} -->
    <h1>POST Test</h1>
    <p>Last value was: {last_value}</p>
    <form method='post' action='post_test.py'>
      <label>Data to send: </label>
      <input type='text' placeholder='Enter data' name='data_to_send' required>
      <input type='submit' value="Submit data"/>
    </form>    
  </body>
</html>
''')