@echo off
@REM This .bat file can be used to run the unit tests.  This .bat will exit with
@REM a '0' for success and a '1' for failure, based on the executation of the
@REM unit test.
@REM Supply the executable filename as the first parameter

@IF NOT EXIST %1 GOTO notFound

%1

@if "%ERRORLEVEL%" == "0" goto good

:bad
@echo "error : Unit tests might have crashed."
@exit 1

:notFound
@echo "File was not found"
@exit 2

:good
@echo "Unit tests ran successfully."
@exit 0
