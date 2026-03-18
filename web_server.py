#!/usr/bin/env python3
"""
Projet Blockchain
Auteur: kheira derdour
"""

import json
import subprocess
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
from urllib.parse import urlparse

ROOT = Path(__file__).resolve().parent
WEB_DIR = ROOT / "web"
BIN_PATH = ROOT / "blockchain"
DB_JSON = ROOT / "blockchain.json"


def run_blockchain_with_input(text: str) -> None:
    if not BIN_PATH.exists():
        raise RuntimeError("Le binaire ./blockchain est introuvable. Lance 'make re'.")
    payload = f"{text}\n:q\n"
    completed = subprocess.run(
        [str(BIN_PATH)],
        input=payload,
        text=True,
        cwd=ROOT,
        capture_output=True,
        timeout=20,
        check=False,
    )
    if completed.returncode != 0:
        raise RuntimeError("Erreur lors de l'ajout du bloc via le programme C.")


def ensure_chain_files() -> None:
    if DB_JSON.exists():
        return
    run_blockchain_with_input("")


class Handler(BaseHTTPRequestHandler):
    def _send_json(self, status: int, payload: dict) -> None:
        body = json.dumps(payload).encode("utf-8")
        self.send_response(status)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def _serve_static(self, file_path: Path) -> None:
        if not file_path.exists() or not file_path.is_file():
            self.send_error(404, "Fichier introuvable")
            return
        suffix = file_path.suffix.lower()
        content_type = "text/plain; charset=utf-8"
        if suffix == ".html":
            content_type = "text/html; charset=utf-8"
        elif suffix == ".css":
            content_type = "text/css; charset=utf-8"
        elif suffix == ".js":
            content_type = "application/javascript; charset=utf-8"

        data = file_path.read_bytes()
        self.send_response(200)
        self.send_header("Content-Type", content_type)
        self.send_header("Content-Length", str(len(data)))
        self.end_headers()
        self.wfile.write(data)

    def do_GET(self):
        parsed = urlparse(self.path)
        path = parsed.path

        if path == "/api/chain":
            try:
                ensure_chain_files()
                payload = json.loads(DB_JSON.read_text(encoding="utf-8"))
                self._send_json(200, payload)
            except Exception as exc:  # noqa: BLE001
                self._send_json(500, {"error": str(exc)})
            return

        if path == "/":
            self._serve_static(WEB_DIR / "index.html")
            return

        safe_path = path.lstrip("/")
        self._serve_static(WEB_DIR / safe_path)

    def do_POST(self):
        parsed = urlparse(self.path)
        if parsed.path != "/api/blocks":
            self.send_error(404, "Endpoint introuvable")
            return

        try:
            length = int(self.headers.get("Content-Length", "0"))
            raw = self.rfile.read(length)
            payload = json.loads(raw.decode("utf-8"))
            data = (payload.get("data") or "").strip()
            if not data:
                self._send_json(400, {"error": "Le texte du bloc est vide."})
                return

            run_blockchain_with_input(data)
            chain_payload = json.loads(DB_JSON.read_text(encoding="utf-8"))
            self._send_json(200, chain_payload)
        except Exception as exc:  # noqa: BLE001
            self._send_json(500, {"error": str(exc)})


def main() -> None:
    server = ThreadingHTTPServer(("127.0.0.1", 8080), Handler)
    print("Serveur web lance sur http://127.0.0.1:8080")
    print("Ctrl+C pour quitter")
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass
    finally:
        server.server_close()


if __name__ == "__main__":
    main()
