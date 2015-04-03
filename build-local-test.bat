@echo off
CLS
SETLOCAL
SET EL=0

cl
IF %ERRORLEVEL% EQU 9009 SET EL=%ERRORLEVEL% && ECHO start from VS command prompt && GOTO ERROR
IF %ERRORLEVEL% NEQ 0 GOTO ERROR
ECHO CL found
ddt
IF %ERRORLEVEL% EQU 9009 SET EL=%ERRORLEVEL% && ECHO ddt not available && GOTO ERROR
ECHO DDT found

IF "%1"=="" ( ECHO no deps path && exit /b 1)

ddt /Q build
IF %ERRORLEVEL% NEQ 0 GOTO ERROR

SET LODEPSDIR=%1
SET PROJ_LIB=%LODEPSDIR%\proj\share
set GDAL_DATA=%LODEPSDIR%\gdal\data
SET PATH=%LODEPSDIR%\geos\lib;%PATH%
SET PATH=%LODEPSDIR%\gdal\lib;%PATH%
SET PATH=%LODEPSDIR%\expat\lib;%PATH%
SET PATH=%LODEPSDIR%\libtiff\lib;%PATH%
SET PATH=%LODEPSDIR%\zlib\lib;%PATH%
::SET PATH=%LODEPSDIR%\osmpbf\lib%PATH%
::SET INCLUDE=%LODEPSDIR%;%INCLUDE%

node -v
node -e "console.log(process.arch,process.execPath)"
npm install --build-from-source --msvs_version=2013 --dist-url=https://s3.amazonaws.com/mapbox/node-cpp11 --toolset=v140
IF %ERRORLEVEL% NEQ 0 GOTO ERROR

GOTO DONE

:ERROR
SET EL=%ERRORLEVEL%
ECHO.
ECHO ====================== ERROR %EL% ======================
ECHO.

:DONE
ECHO ===== DONE ======
EXIT /b %EL%
