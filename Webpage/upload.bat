@echo off
setlocal

:: Prompt for the last digit of the build
set /p destHost=Garden Server IP:
set /p destPass=Garden Server Password:
set /p build=Do you want to build frontend (yes or no):

if %build%==yes (
	echo Building frontend
	npm --prefix backend/ run build
)

:: Define variables
set sourceBackendPath=backend/
set sourceFrontendPath=webpage/build/

set destUser=garden
set destBackendPath=/home/garden/backend/
set destFrontendPath=/home/garden/frontend/

:: Upload the directory to the destination server
echo Uploading Backend directory to %destHost%...
echo %destPass% | pscp -pw %destPass% -r %sourceBackendPath% %destUser%@%destHost%:%destBackendPath%

:: Upload the directory to the destination server
echo Uploading frontend directory to %destHost%...
echo %destPass% | pscp -pw %destPass% -r %sourceFrontendPath% %destUser%@%destHost%:%destFrontendPath%

echo Operation completed.
pause