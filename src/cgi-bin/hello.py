#!/usr/bin/env python3
import os
import cgi

html_text = '<!DOCTYPE html>\n<html>\n<head>\n'
html_text += '\t<title>'+'test'+'</title>\n'
html_text += '\t<meta charset="utf-8">\n'
html_text += '</head>\n\n'
html_text += '<body>\n'

html_text += '<h3>Hello world by python cgi</h3>'

if os.getenv('QUERY_STRING') != None:
    html_text += 'String from browser: ' + os.getenv('QUERY_STRING')

html_text += '</body>\n</html>\n'


print (html_text)

# import sys
# import cgi
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