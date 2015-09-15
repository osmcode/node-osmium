@ECHO off
SETLOCAL
SET EL=0

ECHO ~~~~~~~~~~~~~~~~~~~ %~f0 ~~~~~~~~~~~~~~~~~~~

SET APPVEYOR_REPO_COMMIT_MESSAGE=local testing
::SET APPVEYOR_REPO_COMMIT_MESSAGE=local testing [publish binary]
::SET APPVEYOR_REPO_COMMIT_MESSAGE=local testing [republish binary]
SET nodejs_version=0.10.33
SET platform=x64
SET LOCAL_BUILD=1

SET PATH=C:\Program Files\7-Zip;%PATH%

CALL scripts\build-appveyor.bat
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
