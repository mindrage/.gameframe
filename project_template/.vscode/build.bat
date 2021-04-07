@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
cls

:main
  call :compile_schema data\schemas data\gen\
  call :clear_folders editor
  call :add_folder editor ./
  call :compile_program editor

  echo [End of Build]
goto :eof

:clear_folders
 if not exist .vscode\bin\ (
  mkdir .vscode\bin\
 )

 if not exist .vscode\obj\ (
  mkdir .vscode\obj\
 )

 if exist .vscode\bin\%1.cache (
  del .vscode\bin\%1.cache
 )

goto :eof

      :: echo !B:%CD%\=! >> .vscode\bin\%1.cache
:add_folder
  setlocal enabledelayedexpansion
  set SEARCH_STR=.vscode
  for /R %2 %%f in (*.cpp,*.c) do (
    set B=%%f
    if /I "!B:%SEARCH_STR%=!"=="!B!" (
      echo !B:%CD%\=! >> .vscode\bin\%1.cache
    )
  )

  endlocal
goto :eof


:: (%1 configuration)
:compile_program

 echo [Build] Compiling %1
 
 if not exist .vscode\obj\%1\ (
  mkdir .vscode\obj\%1\
 )

 :: Release via Static with debug info
 cl @.vscode/bin/%1.cache^
 /I ./^
 /I .vscode/3rdparty/enet/include/^
 /I .vscode/3rdparty/cq/^
 /I .vscode/3rdparty/raylib/src/^
 /I .vscode/3rdparty/raylib/src/external^
 /I .vscode/3rdparty/imgui/^
 /I .vscode/3rdparty/^
 /I .vscode/3rdparty/imgui_extra/^
 /I .vscode/3rdparty/spdlog/include/^
 /MD /Os /Oi /GL /Gy /JMC /FS /Zi /nologo /EHsc /std:c++latest^
 /Fe.vscode/bin/%1^
 /Fo.vscode/obj/%1/^
 /Fd.vscode/bin/%1^
 /link /DEBUG:FULL /SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup^
 /LIBPATH:.vscode/3rdparty/build/bin/RelWithDebInfo/^
 raylib.lib shell32.lib opengl32.lib user32.lib gdi32.lib winmm.lib enet.lib ws2_32.lib imgui.lib advapi32.lib spdlog.lib

 echo Finished compilation.
goto :eof


:compile_schema
  setlocal enabledelayedexpansion
  echo [Build] Building schemas
  for /R %1 %%f in (*.fbs) do (
    set B=%%f
    echo !B:%CD%\=!
    call .vscode\3rdparty\build\bin\RelWithDebInfo\flatc.exe -c -o %2 -I %1 --gen-mutable --gen-object-api --gen-compare --scoped-enums --grpc !B:%CD%\=!
  )

  
goto :eof

 