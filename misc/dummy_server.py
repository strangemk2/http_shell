import http.server

response = 'abcdefg'

def http_get(r):
    r.send_response(200)
    r.end_headers()
    r.wfile.write(bytes(response, 'UTF-8'))

def run():
    h = http.server.BaseHTTPRequestHandler
    h.do_GET = http_get
    s = http.server.HTTPServer(('0.0.0.0', 8080), h)
    s.serve_forever()

run()
