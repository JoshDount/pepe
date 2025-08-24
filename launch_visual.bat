@echo off
title Route Transit Simulator - Visual Interface Launcher
color 0A

echo.
echo ╔══════════════════════════════════════════════════════════════╗
echo ║                Route Transit Simulator                      ║
echo ║                   Visual Interface Launcher                 ║
echo ╚══════════════════════════════════════════════════════════════╝
echo.
echo [START] Starting visual interface...
echo.
echo Options available:
echo   1. Web Interface (Recommended) - Interactive browser-based UI
echo   2. Enhanced CLI - Rich command-line interface
echo   3. Exit
echo.

:menu
set /p choice="Select interface (1-3): "

if "%choice%"=="1" goto web_interface
if "%choice%"=="2" goto cli_interface
if "%choice%"=="3" goto exit
echo Invalid choice. Please select 1-3.
goto menu

:web_interface
echo.
echo [WEB] Launching Web Interface...
echo [UI] This will open a modern browser-based interface
echo [FEATURES] Features: Interactive graphs, real-time visualization, dashboards
echo.
echo Starting web server...

cd web
if exist "server.py" (
    echo [OK] Web server found
    echo [LAUNCH] Opening browser interface...
    echo.
    echo [CONTROLS] Controls:
    echo   • Generate graphs with interactive controls
    echo   • Run algorithms with real-time visualization
    echo   • Monitor performance with live dashboards
    echo   • Press Ctrl+C in this window to stop server
    echo.
    echo [URL] Browser will open automatically at http://localhost:8080
    echo.
    start "" "http://localhost:8080"
    python server.py
) else (
    echo [ERROR] Web server not found!
    echo [INFO] Please ensure you're running from the correct directory
    pause
)
goto menu

:cli_interface
echo.
echo [CLI] Launching Enhanced CLI Interface...
echo [INFO] This provides the full-featured command-line interface
echo.
cd..
if exist "build\cli\Debug\route_planner.exe" (
    echo [OK] CLI application found
    echo [LAUNCH] Starting Route Transit Simulator CLI...
    echo.
    echo [COMMANDS] Quick Commands:
    echo   • help - Show all available commands
    echo   • create_graph 100 - Create a test graph
    echo   • dijkstra 1 50 - Find shortest path
    echo   • test_all - Run system tests
    echo   • quit - Exit application
    echo.
    "build\cli\Debug\route_planner.exe"
) else (
    echo [ERROR] CLI application not found!
    echo [BUILD] Please build the project first using: cmake --build build
    pause
)
goto menu

:exit
echo.
echo [EXIT] Thank you for using Route Transit Simulator!
echo.
echo [DOCS] Documentation available at:
echo   • Technical Guide: docs\TECHNICAL_DOCUMENTATION.md
echo   • Deployment Guide: DEPLOYMENT_GUIDE.md
echo   • Performance Analysis: docs\COMPLEXITY_ANALYSIS.md
echo.
echo [STATUS] Project Status: Production Ready [OK]
echo.
pause
exit /b 0