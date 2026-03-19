@echo off
echo Compilation de Silence-Stellaire...
gcc src/*.c -Iinclude -Llib -lSDL3 -lSDL3_image -lSDL3_ttf -lSDL3_mixer -lm -Wall -o bin/Silence-Stellaire.exe

if %ERRORLEVEL% NEQ 0 (
    echo ERREUR: Echec de la compilation !
    pause
    exit /b 1
)

echo Lancement du jeu...
bin\Silence-Stellaire.exe
pause
