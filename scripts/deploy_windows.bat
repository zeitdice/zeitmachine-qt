@ECHO OFF

SETLOCAL
SET "RELEASE_DIR=C:\Users\simonrepp\build-zeitdice-Desktop_Qt_5_6_0_MSVC2015_64bit-Release\release"
SET "FFMPEG_LIB_DIR=C:\Users\simonrepp\zeitmachine\lib\ffmpeg-20160418-git-13406b6-win64-shared\bin"
SET "VC_REDIST_DIR=C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\redist\x64\Microsoft.VC140.CRT"
SET "VC_UCRT_REDIST_DIR=C:\Program Files (x86)\Windows Kits\10\Redist\ucrt\DLLs\x64"
SET "QT_REDIST_DIR=C:\Qt\5.6\msvc2015_64\bin"


REM Clean from any compilation artifacts
del /s/q "%RELEASE_DIR%\*.cpp"
del /s/q "%RELEASE_DIR%\*.obj"
del /s/q "%RELEASE_DIR%\*.res"

REM Remove (possibly weirdly set) zeitdice setting files
del /s/q "%RELEASE_DIR%\.zeit*"

REM Copy Qt libraries
%QT_REDIST_DIR%\windeployqt.exe "%RELEASE_DIR%\zeitdice.exe" --no-opengl-sw --no-translations

REM Remove unneeded Qt5Svg.dll
del /s/q "%RELEASE_DIR%\Qt5Svg.dll"

REM Remove unneeded Qt imageformats and iconengines plugins
rd /s/q "%RELEASE_DIR%\imageformats"
rd /s/q "%RELEASE_DIR%\iconengines"

REM Copy ffmpeg libraries
xcopy /s/y "%FFMPEG_LIB_DIR%\*.dll" %RELEASE_DIR%

REM Copy Visual Studio 14.0 VC(++) runtime libraries
xcopy /s/y "%VC_REDIST_DIR%\concrt140.dll" %RELEASE_DIR%
xcopy /s/y "%VC_REDIST_DIR%\msvcp140.dll" %RELEASE_DIR%
xcopy /s/y "%VC_REDIST_DIR%\vccorlib140.dll" %RELEASE_DIR%
xcopy /s/y "%VC_REDIST_DIR%\vcruntime140.dll" %RELEASE_DIR%
xcopy /s/y "%VC_UCRT_REDIST_DIR%\*.dll" %RELEASE_DIR%



REM qt icu dlls
REM (update - not needed: http://doc.qt.io/qt-5/qcollator.html#setNumericMode)
REM xcopy /s/y "C:\Qt\5.6\msvc2015_64\bin\icu*.dll" %RELEASE_DIR%
