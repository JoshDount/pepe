@echo off
title Route Transit Simulator - Web Interface
color 0B

echo.
echo ╔══════════════════════════════════════════════════════════════╗
echo ║                Route Transit Simulator                      ║
echo ║                      Web Interface                          ║
echo ╚══════════════════════════════════════════════════════════════╝
echo.
echo [WEB] Starting web interface...
echo [URL] URL: http://localhost:8080
echo [AUTO] Browser will open automatically
echo.
echo [STOP] Press Ctrl+C to stop the server
echo.

REM Change to web directory
cd /d "%~dp0\web"

REM Check if required files exist
if not exist "index.html" (
    echo [ERROR] Error: Web files not found!
    echo [INFO] Please ensure index.html exists in /web/ directory
    pause
    exit /b 1
)

if not exist "server.py" (
    echo [WARNING] Warning: server.py not found, opening static HTML file
    start "" "index.html"
    echo [OK] Static web page opened in browser
    pause
    exit /b 0
)

REM Start the web server
echo [OK] Starting Python web server...
echo.

REM Open browser automatically after a short delay
start /min cmd /c "timeout /t 2 >nul && start http://localhost:8080"

REM Run the Python server
python server.py

echo.
echo [EXIT] Web server stopped.
pause