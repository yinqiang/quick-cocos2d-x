@echo off
set DIR=%~dp0
cd "%DIR%\..\"
copy /Y player\proj.win32\Debug\*.exe player\bin\win32\
copy /Y player\proj.win32\Debug\*.dll player\bin\win32\
copy /Y player\proj.win32\Debug\*.lib player\bin\win32\
