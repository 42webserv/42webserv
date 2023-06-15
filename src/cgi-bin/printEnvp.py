#!/usr/bin/python3

import asyncio
import os

async def my_coroutine():
    while True:
        # 작업 수행
        await asyncio.sleep(1)  # 작업 시간

async def timeout_task():
    try:
        await asyncio.wait_for(my_coroutine(), timeout=3)
    except asyncio.TimeoutError:
        # 무한 루프가 도는 상황에서 타임아웃 발생
        # 원하는 오류 메시지 출력 및 종료 처리
        print("Error: Infinite loop detected")
        return

while 1:
    loop = asyncio.get_event_loop()
    loop.run_until_complete(timeout_task())
    loop.close()

    # 나머지 코드 (Content-Type 헤더 출력, 환경 변수 출력 등)
    print("Status: 200 OK")
    print("Content-Type: text/html; charset=utf-8\r\n\r\n<html><body>")
    print()

    if loop.is_closed():
        for param in os.environ.keys():
            print("<b>{}</b>: {}<br>".format(param, os.environ[param]))

    print("</body></html>")

