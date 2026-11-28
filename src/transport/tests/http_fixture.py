"""Synthetic loopback server; no account, token, or live service access."""
import http.server
import os
import subprocess
import sys
import threading

cancelled = threading.Event()


class Handler(http.server.BaseHTTPRequestHandler):
    protocol_version = 'HTTP/1.1'

    def log_message(self, *args):
        pass

    def respond(self):
        if self.path == '/cancelled':
            cancelled.set()
        payload = self.rfile.read(int(self.headers.get('Content-Length', '0')))
        if self.path in ('/large', '/exact'):
            body = b'x' * (128 if self.path == '/large' else 16)
        else:
            body = (self.command + '|' + self.path + '|' + self.headers.get('X-Test', '') + '|').encode() + payload
        valid = 'Cookie' not in self.headers and (self.path in ('/large', '/exact') or 'X-Empty' in self.headers)
        self.send_response(200 if valid else 400)
        self.send_header('Content-Length', str(len(body)))
        self.send_header('X-Request-Id', 'fixture')
        self.send_header('Retry-After', '2')
        self.send_header('Set-Cookie', 'session=fixture')
        self.end_headers()
        if self.command != 'HEAD':
            try:
                self.wfile.write(body)
            except (BrokenPipeError, ConnectionResetError):
                pass

    do_GET = do_HEAD = do_POST = do_PUT = do_PATCH = do_DELETE = respond


with http.server.ThreadingHTTPServer(('127.0.0.1', 0), Handler) as server:
    thread = threading.Thread(target=server.serve_forever, daemon=True)
    thread.start()
    environment = os.environ.copy()
    for name in ('HTTP_PROXY', 'HTTPS_PROXY', 'ALL_PROXY', 'http_proxy', 'https_proxy', 'all_proxy'):
        environment[name] = 'http://127.0.0.1:1'
    environment['NO_PROXY'] = environment['no_proxy'] = ''
    try:
        subprocess.run([sys.argv[1], 'http://127.0.0.1:' + str(server.server_port)], env=environment, check=True, timeout=20)
        if cancelled.is_set():
            raise AssertionError('A cancelled transfer performed HTTP')
    finally:
        server.shutdown()
        thread.join(timeout=5)
