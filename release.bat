@echo off
set projectdir=%CD%

IF [%1]==[] (
	echo Must supply target directory
	exit /B 1
)

set releasedir=%1
set releasedir=%releasedir:"=%

REM StdUtils
set targetdir=%releasedir%
IF exist "%targetdir%" ( 
	rmdir "%targetdir%" /S /Q
)
mkdir "%targetdir%"
mkdir "%targetdir%\Source"

REM public headers
xcopy "%projectdir%\StdUtils" "%targetdir%\StdUtils" /S /I /Y
REM private headers
xcopy "%projectdir%\private" "%targetdir%\private" /S /I /Y
REM sources
for /R "%projectdir%" %%f in (*.cpp) do copy "%%f" "%targetdir%\Source"
REM CMake
copy CMakeLists.txt "%targetdir%\"