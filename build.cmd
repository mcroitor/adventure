@echo off
rem variable block
set CC=g++
set CFLAGS=-g -Wall -Wextra -Werror -std=c++17

set SRCDIR=src
set BINDIR=bin
set OBJDIR=obj
set APPNAME=adventure

rem Stage 1
echo Create infrastructure.
mkdir %BINDIR%
mkdir %OBJDIR%
echo infrastructure is created.

rem Stage 2
echo Compile CPP files.
%CC% %SRCDIR%\Chest.cpp -o %OBJDIR%\Chest.o -c %CFLAGS%
%CC% %SRCDIR%\Direction.cpp -o %OBJDIR%\Direction.o -c %CFLAGS%
%CC% %SRCDIR%\Engine.cpp -o %OBJDIR%\Engine.o -c %CFLAGS%
%CC% %SRCDIR%\Item.cpp -o %OBJDIR%\Item.o -c %CFLAGS%
%CC% %SRCDIR%\Map.cpp -o %OBJDIR%\Map.o -c %CFLAGS%
%CC% %SRCDIR%\Obstacle.cpp -o %OBJDIR%\Obstacle.o -c %CFLAGS%
%CC% %SRCDIR%\main.cpp -o %OBJDIR%\main.o -c %CFLAGS%
echo All CPP files are compiled.

rem Stage 3
echo Build app %APPNAME%.
%CC% %OBJDIR%\Chest.o ^
    %OBJDIR%\Direction.o ^
    %OBJDIR%\Engine.o ^
    %OBJDIR%\Item.o ^
    %OBJDIR%\Map.o ^
    %OBJDIR%\Obstacle.o ^
    %OBJDIR%\main.o ^
    -o %BINDIR%\%APPNAME%
echo %APPNAME% is created.