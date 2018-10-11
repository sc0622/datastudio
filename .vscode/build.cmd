@echo off
title qbs build prompt
set VCINSTALLDIR=C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC
set QTDIR=C:\Qt\Qt5.11.2\5.11.2\msvc2015
set PATH=%VCINSTALLDIR%\bin;%QTDIR%\bin;C:\Program Files\7-Zip;%PATH%
call "%VCINSTALLDIR%\vcvarsall.bat" x86
qbs datastudio.qbs -spec win32-msvc "CONFIG+=debug" "CONFIG+=qml_debug"
nmake
cd debug
windeployqt datastudiod.exe
