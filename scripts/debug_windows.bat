@ECHO OFF

SETLOCAL
SET "PROJECT_DIR=%~dp0.."
SET "ZEITDICE_DIR=%PROJECT_DIR%\.."
SET "BUILD_DIR=%ZEITDICE_DIR%\build-zeitmachine-Desktop-Debug\debug"
SET "FFMPEG_LIB_DIR=%PROJECT_DIR%\dependencies\ffmpeg-3.3.3-win64-shared\bin"
SET "VC_REDIST_DIR=C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Redist\MSVC\14.11.25325\x64\Microsoft.VC141.CRT"
SET "VC_UCRT_REDIST_DIR=C:\Program Files (x86)\Windows Kits\10\Redist\ucrt\DLLs\x64"
SET "QT_REDIST_DIR=C:\Qt\5.9.1\msvc2017_64\bin"

REM Clean from any compilation artifacts
del /s/q "%BUILD_DIR%\*.cpp"
del /s/q "%BUILD_DIR%\*.h"
del /s/q "%BUILD_DIR%\*.obj"
del /s/q "%BUILD_DIR%\*.res"

REM Remove (possibly weirdly set) zeitdice setting files
del /s/q "%BUILD_DIR%\.zeit*"

REM Copy Qt libraries
%QT_REDIST_DIR%\windeployqt.exe "%BUILD_DIR%\zeitmachine.exe" --no-opengl-sw --no-translations

REM Remove unneeded Qt5Svg.dll
del /s/q "%BUILD_DIR%\Qt5Svg.dll"

REM Remove unneeded Qt imageformats and iconengines plugins
rd /s/q "%BUILD_DIR%\imageformats"
rd /s/q "%BUILD_DIR%\iconengines"

REM Copy ffmpeg libraries
xcopy /s/y "%FFMPEG_LIB_DIR%\*.dll" %BUILD_DIR%

REM Copy Visual Studio 14.0 VC(++) runtime libraries
xcopy /s/y "%VC_REDIST_DIR%\concrt140.dll" %BUILD_DIR%
xcopy /s/y "%VC_REDIST_DIR%\msvcp140.dll" %BUILD_DIR%
xcopy /s/y "%VC_REDIST_DIR%\vccorlib140.dll" %BUILD_DIR%
xcopy /s/y "%VC_REDIST_DIR%\vcruntime140.dll" %BUILD_DIR%
xcopy /s/y "%VC_UCRT_REDIST_DIR%\*.dll" %BUILD_DIR%
