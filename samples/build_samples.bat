echo off
for /L %%a in (1,1,25) do call :makesample %%a
goto :EOF

:makesample
echo %1
cd sample_%1
..\..\bin\rihtan Hello.rihtan
gcc -Wall *.c -o Hello.exe
cd ..
goto :EOF
