@echo off

REM Import environment variables for Visual Studio.
call vcvars32.bat

REM Tell qmake to build the Makefile which will be used to generate the sources.
call %QTDIR%\bin\qmake -makefile editor.pro

REM Now, since we will be actually building the application using Visual Studio,
REM we only use the Makefile to generate the MOC sources.  Since these are added
REM to the VC++ project, they will get included in the final build.
call nmake debug-mocables

