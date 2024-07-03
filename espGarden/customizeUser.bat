@echo off
setlocal enabledelayedexpansion

:: Fetch the username from the environment variables
set "username=%USERNAME%"

:: Specify the file path (replace 'path\to\your\file.txt' with your actual file path)
set "file=.vscode\settings.json"

:: Temp file creation for processing
set "tempfile=%file%.tmp"

:: Check if the file exists
if not exist "!file!" (
    echo File not found
    goto :eof
)

:: Replace "USERNAME" with the actual username in the file
(for /f "delims=" %%i in ('type "!file!"') do (
    set "line=%%i"
    set "modified=!line:USERNAME=%username%!"
    echo !modified!
)) > "!tempfile!"

:: Move the temp file to original file
move /Y "!tempfile!" "!file!" > NUL

echo Replacement complete.
endlocal
