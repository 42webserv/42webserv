#!/usr/bin/python3

import cgi
import os
from urllib.parse import quote

# 허용된 파일 확장자
ALLOWED_EXTENSIONS = {'txt', 'cpp'} # 파일 형식

def allowed_file(filename):
    return '.' in filename and \
           filename.rsplit('.', 1)[1].lower() in ALLOWED_EXTENSIONS

form = cgi.FieldStorage()

fileitem = form['file1']

path = "./src/cgi-bin/file"

isExist = os.path.exists(path)

if not isExist:
    os.makedirs(path)

if fileitem.filename:
    if allowed_file(fileitem.filename):
        encoded_filename = quote(fileitem.filename, encoding='utf-8')
        fn = os.path.basename(encoded_filename)
        with open(os.path.join(path, fn), 'wb') as f:
            f.write(fileitem.file.read())
        message = "The file '{}' was uploaded successful".format(fn)
    else:
        message = "Invalid file extension. Only the following file extensions are allowed: {}".format(', '.join(ALLOWED_EXTENSIONS))
else:
    message = "No file was uploaded"

print("Content-Type: text/html; charset=utf-8")
print()
print("""\
<html><body>
<p>{}</p>
</body></html>
""".format(message))
