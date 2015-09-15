@ECHO off
SETLOCAL
SET EL=0

ECHO ~~~~~~~~~~~~~~~~~~~ %~f0 ~~~~~~~~~~~~~~~~~~~

IF NOT DEFINED LOCAL_BUILD SET LOCAL_BUILD=0

CALL "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64
IF %ERRORLEVEL% NEQ 0 GOTO ERROR

SET LODEPSDIR=%CD%\libosmium-deps
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

powershell Start-FileDownload https://mapbox.s3.amazonaws.com/windows-builds/windows-build-deps/libosmium-deps-win-14.0-x64.7z -FileName lodeps.7z
IF %ERRORLEVEL% NEQ 0 GOTO ERROR
powershell Start-FileDownload https://mapbox.s3.amazonaws.com/node-cpp11/v$env:nodejs_version/x64/node.exe -FileName node.exe
IF %ERRORLEVEL% NEQ 0 GOTO ERROR

7z x -y lodeps.7z | %windir%\system32\FIND "ing archive"
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


install:
  - CALL "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64
  - SET LODEPSDIR=c:\projects\node-osmium\libosmium-deps
  - SET PROJ_LIB=%LODEPSDIR%\proj\share
  - set GDAL_DATA=%LODEPSDIR%\gdal\data
  #geos.dll
  - SET PATH=%LODEPSDIR%\geos\lib;%PATH%
  #gdal.dll
  - SET PATH=%LODEPSDIR%\gdal\lib;%PATH%
  #libexpat.dll
  - SET PATH=%LODEPSDIR%\expat\lib;%PATH%
  #libtiff.dll
  - SET PATH=%LODEPSDIR%\libtiff\lib;%PATH%
  #zlibwapi.dll
  - SET PATH=%LODEPSDIR%\zlib\lib;%PATH%
  - SET PATH=%CD%;%PATH%;
  - ps: Start-FileDownload https://mapbox.s3.amazonaws.com/windows-builds/windows-build-deps/libosmium-deps-win-14.0-x64.7z -FileName lodeps.7z
  - ps: Start-FileDownload "https://mapbox.s3.amazonaws.com/node-cpp11/v0.10.33/x64/node.exe"
  - 7z x lodeps.7z > nul
  - echo %LODEPSDIR%
  - dir %LODEPSDIR%
  - git clone https://github.com/osmcode/libosmium.git ../libosmium

build_script:
  - cd c:\projects\node-osmium
  - node -v
  - node -e "console.log(process.arch,process.execPath)"
  - npm install --build-from-source --msvs_version=2013 --dist-url=https://s3.amazonaws.com/mapbox/node-cpp11 --toolset=v140
  - npm test

test: off

deploy: off

