#!/usr/bin/python3
import os
import cgi

# Content-Type 헤더 출력
print("Content-Type: text/html")
print()

# HTML 페이지 출력
print("<html><body>")

# 환경 변수 출력
for param in os.environ.keys():
    print("<b>{}</b>: {}<br>".format(param, os.environ[param]))

print("</body></html>")