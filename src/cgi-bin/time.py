#!/usr/bin/python3
import cgi

from datetime import datetime
current_time = datetime.now()


print("<!DOCTYPE html>")
print("<html lang=\"en\">")
print("<head>")
print("	<meta charset=\"UTF-8\">")
print("	<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">")
print("	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">")
print("	<title>TestCGI</title>")
print("</head>")
print("<body>")
print("	<h1>Hello, World!</h1>")
print(current_time)
# 새로고침할때 마다 시간 표시
# form = cgi.FieldStorage()
# id = form.getvalue('id')
# name = form.getvalue('name')

   
# print("<p>ID: {}</p>".format(id))
# print("<p>Name: {}</p>".format(name))
# print("</body>")
# print("</html>")
#추가 기능 여부에 따라 주석처리