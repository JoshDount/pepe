#!/usr/bin/env bash
set -euo pipefail

echo "==============================================="
echo "  Route Transit Simulator - Web Interface"
echo "==============================================="
echo "[WEB] Starting web interface..."
echo "[URL] http://localhost:8080"
echo

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

# Ensure React UI is built
echo "[CHECK] Verifying React UI build files..."
UI_DIR="$SCRIPT_DIR/web/route-planner-ui"

if [ ! -d "$UI_DIR/node_modules" ]; then
  echo "[INSTALL] Installing UI dependencies..."
  (cd "$UI_DIR" && npm install)
fi

if [ ! -f "$UI_DIR/dist/index.html" ]; then
  echo "[BUILD] Building React UI for production..."
  (cd "$UI_DIR" && npm run build)
fi

# Start Python server
cd "$SCRIPT_DIR/web"
echo "[OK] Starting Python web server..."
echo "[HINT] Press Ctrl+C to stop"

if command -v python3 >/dev/null 2>&1; then
  python3 server.py
else
  python server.py
fi

