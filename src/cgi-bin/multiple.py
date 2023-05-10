import cgi
form = cgi.FieldStorage()

a = form.getvalue ('a')
b = form.getvalue ('b')

if a is not None and b is not None:
    result = int(a) * int(b)
    print("Result:", result)
else:
    print("Error: Both 'a' and 'b' values must be provided.")

#result = int(a) * int(b)

# print('Content - type: text/plain')
# print()
# print(f'Result:{result}')
