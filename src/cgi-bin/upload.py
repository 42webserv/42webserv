#!/usr/bin/python3

import cgi
import os

# 업로드된 파일이 저장된 디렉토리 경로
upload_dir = "./src/cgi-bin/file"

# Content-Type 헤더 출력
print("Content-Type: text/html")
print()

# HTML 페이지 출력
print("<html><body>")

# CGI 데이터 가져오기
form = cgi.FieldStorage()

# 업로드된 파일 처리
if "file1" in form:
    fileitem = form["file1"]

    # 파일 저장 경로 설정
    filepath = os.path.join(upload_dir, fileitem.filename)

    # 파일 저장
    with open(filepath, "wb") as f:
        f.write(fileitem.file.read())

    # 파일 존재 여부 확인
    if os.path.exists(filepath):
        print("<p>file upload success.</p>")
        print("<p>file path: {}</p>".format(filepath))
    else:
        print("<p>file upload failed.</p>")
else:
    print("<p>not file.</p>")

print("</body></html>")

