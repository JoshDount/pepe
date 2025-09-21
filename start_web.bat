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

REM Ensure React UI is built
echo [CHECK] Verifying React UI build files...
cd /d "%~dp0\web\route-planner-ui"

if not exist "node_modules" (
    echo [INSTALL] Installing UI dependencies (this can take a moment)...
    npm install
)

if not exist "dist\index.html" (
    echo [BUILD] Building React UI for production...
    npm run build
)

REM Return to web folder to start Python server
cd /d "%~dp0\web"

if not exist "server.py" (
    echo [ERROR] Error: server.py not found in /web/
    echo [INFO] Please ensure web/server.py exists
    pause
    exit /b 1
)

REM Start the web server
echo [OK] Starting Python web server...
echo.

REM Open browser automatically after a short delay
start /min cmd /c "timeout /t 2 >nul && start http://localhost:8080"

REM Run the Python server (prefer Python 3)
where py >nul 2>nul && (py -3 server.py) || (python server.py)

echo.
echo [EXIT] Web server stopped.
pause
