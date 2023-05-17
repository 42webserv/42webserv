#!/usr/bin/env python3
# import os

# html_text = '<!DOCTYPE html>\n<html>\n<head>\n'
# html_text += '\t<title>'+'test'+'</title>\n'
# html_text += '\t<meta charset="utf-8">\n'
# html_text += '</head>\n\n'
# html_text += '<body>\n'

# html_text += '<h3>Hello world by python cgi</h3>'

# if os.getenv('QUERY_STRING') != None:
#     html_text += 'String from browser: ' + os.getenv('QUERY_STRING')

# html_text += '</body>\n</html>\n'

# print (html_text)

import sys
import cgi
# print("\naaaaaaaaaaaaaaaaaaaaaaa\n")
# print("Content-type:text/html\r\n\r\n")

# print("<html>")
# print("<head>")
# print("<title>Hello, World!</title>")
# print("</head>")
# print("<body>")
# print("<h2>Hello, World!</h2>")

# form = cgi.FieldStorage()
# id = form.getvalue('id')
# name = form.getvalue('name')

# print("<p>ID: {}</p>".format(id))
# print("<p>Name: {}</p>".format(name))

# print("</body>")
# print("</html>")

# from datetime import datetime
# import os
# current_time = datetime.now()


# print("<!DOCTYPE html>")
# print("<html lang=\"en\">")
# print("<head>")
# print("	<meta charset=\"UTF-8\">")
# print("	<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">")
# print("	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">")
# print("	<title>TestCGI</title>")
# print("</head>")
# print("<body>")
# print("	<h1>Hello, World!</h1>")
# print("	<h2>aaaaaaaaaaaaa</h2>")
# print(current_time)
# # 새로고침할때 마다 시간 표시
# form = cgi.FieldStorage()
# id = form.getvalue('id')
# name = form.getvalue('name')

# print ("Content-type: text/html\r\n\r\n");
# print ("<font size=+1>Environment</font></br>");

# for param in os.environ.keys():
#    print ("<b>%20s</b>: %s</br>" % (param, os.environ[param]))
   
# print("<p>ID: {}</p>".format(id))
# print("<p>Name: {}</p>".format(name))
# print("</body>")
# print("</html>")


import os

# Content-Type 헤더 출력
print("Content-Type: text/html")
print()

# HTML 페이지 출력
print("<html><body>")

# 환경 변수 출력
for param in os.environ.keys():
    print("<b>{}</b>: {}<br>".format(param, os.environ[param]))

print("</body></html>")
# #!/usr/bin/python3
# import cgi
# form = cgi.FieldStorage()

# print("\naaaaaaaaaaaaaaaaaaaaaaa\n")

# a = form.getvalue('a')
# b = form.getvalue('b')

# result = int(a) * int(b)

# print('Content-type: text/plain')
# print()
# print(f'Result:{result}')
