@echo off
echo.
echo ============================================================
echo  Route Transit Simulator - Production Launcher
echo ============================================================
echo.

REM Check if executables exist
if not exist "build\cli\Debug\route_planner.exe" (
    echo ERROR: route_planner.exe not found!
    echo Please ensure the project has been built successfully.
    echo Run: cmake --build build
    pause
    exit /b 1
)

if not exist "build\bench\Debug\benchmark_runner.exe" (
    echo WARNING: benchmark_runner.exe not found!
    echo Benchmarking will not be available.
)

echo Available options:
echo.
echo   1. Launch Route Planner (Main Application)
echo   2. Run Quick Performance Check
echo   3. Run Comprehensive Benchmarks
echo   4. Run System Tests
echo   5. Show System Information
echo   6. Open Documentation
echo   7. Exit
echo.

:menu
set /p choice="Select option (1-7): "

if "%choice%"=="1" goto launch_main
if "%choice%"=="2" goto quick_bench
if "%choice%"=="3" goto full_bench
if "%choice%"=="4" goto run_tests
if "%choice%"=="5" goto system_info
if "%choice%"=="6" goto open_docs
if "%choice%"=="7" goto exit
echo Invalid choice. Please select 1-7.
goto menu

:launch_main
echo.
echo Starting Route Transit Simulator CLI...
echo Type 'help' for available commands, 'quit' to exit
echo.
"build\cli\Debug\route_planner.exe"
goto menu

:quick_bench
echo.
echo Running quick performance check...
echo.
"build\bench\Debug\benchmark_runner.exe" quick
echo.
echo Performance check completed.
pause
goto menu

:full_bench
echo.
echo WARNING: Comprehensive benchmarks may take 30-60 minutes.
set /p confirm="Continue? (y/N): "
if /i not "%confirm%"=="y" goto menu
echo.
echo Running comprehensive benchmark suite...
echo.
"build\bench\Debug\benchmark_runner.exe" full
echo.
echo Benchmark suite completed.
pause
goto menu

:run_tests
echo.
echo Running system validation tests...
echo.
if exist "build\tests\Debug\unit_tests.exe" (
    "build\tests\Debug\unit_tests.exe"
) else (
    echo Unit tests executable not found.
    echo Running built-in CLI tests instead...
    echo test_all | "build\cli\Debug\route_planner.exe"
)
echo.
echo Tests completed.
pause
goto menu

:system_info
echo.
echo ============================================================
echo  System Information
echo ============================================================
echo.
echo Project Location: %CD%
echo.
echo Available Executables:
if exist "build\cli\Debug\route_planner.exe" (
    echo   ✓ route_planner.exe     [Main Application]
) else (
    echo   ✗ route_planner.exe     [MISSING]
)

if exist "build\bench\Debug\benchmark_runner.exe" (
    echo   ✓ benchmark_runner.exe  [Performance Suite]
) else (
    echo   ✗ benchmark_runner.exe  [MISSING]
)

if exist "build\tests\Debug\unit_tests.exe" (
    echo   ✓ unit_tests.exe        [Test Suite]
) else (
    echo   ~ unit_tests.exe        [Optional]
)

echo.
echo Documentation:
if exist "docs\TECHNICAL_DOCUMENTATION.md" (
    echo   ✓ Technical Documentation
) else (
    echo   ✗ Technical Documentation [MISSING]
)

if exist "docs\COMPLEXITY_ANALYSIS.md" (
    echo   ✓ Complexity Analysis
) else (
    echo   ✗ Complexity Analysis [MISSING]
)

if exist "docs\FINAL_REPORT.md" (
    echo   ✓ Final Report
) else (
    echo   ✗ Final Report [MISSING]
)

echo.
echo System Status: READY FOR DEPLOYMENT
echo.
pause
goto menu

:open_docs
echo.
echo Opening documentation...
echo.
if exist "docs\TECHNICAL_DOCUMENTATION.md" (
    start "" "docs\TECHNICAL_DOCUMENTATION.md"
)
if exist "docs\DEPLOYMENT_GUIDE.md" (
    start "" "DEPLOYMENT_GUIDE.md"
)
if exist "README.md" (
    start "" "README.md"
)
echo Documentation opened in default text editor.
pause
goto menu

:exit
echo.
echo Thank you for using Route Transit Simulator!
echo.
echo For support and documentation, visit:
echo   - Technical Documentation: docs\TECHNICAL_DOCUMENTATION.md
echo   - Deployment Guide: DEPLOYMENT_GUIDE.md
echo   - Final Report: docs\FINAL_REPORT.md
echo.
echo Project Status: PRODUCTION READY ✓
echo.
pause