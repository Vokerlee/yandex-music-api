"""Serve binary audio and interrupted transfers for the download integration test."""
import http.server
from pathlib import Path
import subprocess
import sys
import tempfile
import threading

AUDIO = (b"\0MUSIC\xff" * (9 * 1024 * 1024 // 7 + 2))[:9 * 1024 * 1024 + 7]


class Handler(http.server.BaseHTTPRequestHandler):
    def log_message(self, *args):
        pass

    def do_GET(self):
        self.send_response(400 if "Authorization" in self.headers else 200)
        self.send_header("Content-Length", str(len(AUDIO)))
        self.end_headers()
        try:
            self.wfile.write(AUDIO[:1024] if self.path == "/truncated" else AUDIO)
        except (BrokenPipeError, ConnectionResetError):
            pass


with tempfile.TemporaryDirectory() as directory, http.server.ThreadingHTTPServer(("127.0.0.1", 0), Handler) as server:
    thread = threading.Thread(target=server.serve_forever, daemon=True)
    thread.start()
    try:
        subprocess.run([sys.argv[1], f"http://127.0.0.1:{server.server_port}", str(Path(directory) / "track.bin")], check=True, timeout=30)
    finally:
        server.shutdown()
        thread.join(timeout=5)
