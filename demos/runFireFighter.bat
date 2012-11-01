@echo off

set PATH=%PATH%;%DELTA_ROOT%\bin;%DELTA_ROOT%\ext\bin;..\bin;
set APP_LIB=fireFighter

cd %DELTA_ROOT%

call GameStart.exe %APP_LIB%