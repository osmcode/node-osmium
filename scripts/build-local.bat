@ECHO off
SETLOCAL
SET EL=0

ECHO ~~~~~~~~~~~~~~~~~~~ %~f0 ~~~~~~~~~~~~~~~~~~~

SET nodejs_version=0.10.33
SET platform=x64

SET PATH=C:\Program Files\7-Zip;%PATH%

GOTO DONE

:ERROR
SET EL=%ERRORLEVEL%
ECHO ~~~~~~~~~~~~~~~~~~~ ERROR %~f0 ~~~~~~~~~~~~~~~~~~~
ECHO ERRORLEVEL^: %EL%

:DONE
ECHO ~~~~~~~~~~~~~~~~~~~ DONE %~f0 ~~~~~~~~~~~~~~~~~~~


CD %ROOTDIR%
EXIT /b %EL%
