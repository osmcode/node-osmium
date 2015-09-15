@ECHO off
SETLOCAL
SET EL=0

ECHO ~~~~~~~~~~~~~~~~~~~ %~f0 ~~~~~~~~~~~~~~~~~~~

IF NOT DEFINED LOCAL_BUILD SET LOCAL_BUILD=0
ECHO LOCAL_BUILD^: %LOCAL_BUILD%

CALL "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64
IF %ERRORLEVEL% NEQ 0 GOTO ERROR

IF %LOCAL_BUILD% EQU 1 (SET LODEPSDIR=%CD%\..\libosmium-deps\libosmium-deps) ELSE (SET LODEPSDIR=%CD%\libosmium-deps)
SET PROJ_LIB=%LODEPSDIR%\proj\share
SET GDAL_DATA=%LODEPSDIR%\gdal\data
SET PATH=%LODEPSDIR%\geos\lib;%PATH%
SET PATH=%LODEPSDIR%\gdal\lib;%PATH%
SET PATH=%LODEPSDIR%\expat\lib;%PATH%
SET PATH=%LODEPSDIR%\libtiff\lib;%PATH%
SET PATH=%LODEPSDIR%\zlib\lib;%PATH%
SET PATH=%CD%;%PATH%;

IF EXIST lodeps.7z DEL lodeps.7z
IF %ERRORLEVEL% NEQ 0 GOTO ERROR

IF EXIST node.exe DEL node.exe
IF %ERRORLEVEL% NEQ 0 GOTO ERROR

IF %LOCAL_BUILD% NEQ 1 powershell Invoke-WebRequest https://mapbox.s3.amazonaws.com/windows-builds/windows-build-deps/libosmium-deps-win-14.0-x64.7z -OutFile lodeps.7z
IF %ERRORLEVEL% NEQ 0 GOTO ERROR
powershell Invoke-WebRequest https://mapbox.s3.amazonaws.com/node-cpp11/v$env:nodejs_version/x64/node.exe -OutFile node.exe
IF %ERRORLEVEL% NEQ 0 GOTO ERROR

IF %LOCAL_BUILD% NEQ 1 7z x -y lodeps.7z | %windir%\system32\FIND "ing archive"
IF %ERRORLEVEL% NEQ 0 GOTO ERROR

IF %LOCAL_BUILD% EQU 1 IF EXIST ..\libosmium (ECHO DELETING previously cloned libosmium && RD /Q /S ..\libosmium)
IF %ERRORLEVEL% NEQ 0 GOTO ERROR

git clone https://github.com/osmcode/libosmium.git ../libosmium
IF %ERRORLEVEL% NEQ 0 GOTO ERROR

node -v
IF %ERRORLEVEL% NEQ 0 GOTO ERROR
node -e "console.log(process.arch,process.execPath)"
IF %ERRORLEVEL% NEQ 0 GOTO ERROR

CALL npm install ^
--build-from-source ^
--msvs_version=2015 ^
--dist-url=https://s3.amazonaws.com/mapbox/node-cpp11 ^
--toolset=v140
IF %ERRORLEVEL% NEQ 0 GOTO ERROR

CALL npm test
IF %ERRORLEVEL% NEQ 0 GOTO ERROR



GOTO DONE

:ERROR
SET EL=%ERRORLEVEL%
ECHO ~~~~~~~~~~~~~~~~~~~ ERROR %~f0 ~~~~~~~~~~~~~~~~~~~
ECHO ERRORLEVEL^: %EL%

:DONE
ECHO ~~~~~~~~~~~~~~~~~~~ DONE %~f0 ~~~~~~~~~~~~~~~~~~~

CD %ROOTDIR%
EXIT /b %EL%
