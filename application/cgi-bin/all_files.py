import os

# Ref: https://stackoverflow.com/questions/3207219/how-do-i-list-all-files-of-a-directory

application_directory = "application"
save_folder_name = "uploaded-files"
files_directory = os.path.join(os.getcwd(), application_directory, save_folder_name)
all_files = []
if (os.path.exists(files_directory)):
    all_files = [f for f in os.listdir(files_directory) if os.path.isfile(os.path.join(files_directory, f))]

print('''
<html>
  <body>
    <h1>All files</h1>   
    <ul>
''')
for file in all_files:
  print(f'''
      <li><a href={"/uploaded-files/" + file}>{file}</a></li>
        ''')
  
if not all_files:
   print('''
      <li>No files in the directory</li>
''')

print('''
    </ul>
    <p><a href="/cgi-bin/upload_file.py">Upload a new file</a></p>
  </body>
</html>
''')