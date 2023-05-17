# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    upload.py                                          :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: yje <yje@student.42seoul.kr>               +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/05/17 13:17:19 by yje               #+#    #+#              #
#    Updated: 2023/05/17 14:40:14 by yje              ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#!/usr/bin/python3

import cgi
import os

# 업로드된 파일이 저장된 디렉토리 경로
upload_dir = "/src/cgi-bin/upload"

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
        print("<p>파일 업로드가 완료되었습니다.</p>")
        print("<p>업로드된 파일 경로: {}</p>".format(filepath))
    else:
        print("<p>파일을 업로드하는 동안 오류가 발생했습니다.</p>")
else:
    print("<p>파일을 찾을 수 없습니다.</p>")

print("</body></html>")

