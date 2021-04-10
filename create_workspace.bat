@echo off

:main
 call :add_include bgfx\include\
 call :add_include bimg\include\
 call :add_include bx\include\
 call :add_include bx\include\compat\msvc\
 call :add_include enet\include\
 call :add_include flecs\include\
 call :add_include gameframe\ gameframe\
 call :add_include glm\glm\ glm\
 call :add_include imgui\ imgui\ "*.h *.hpp /XD examples"
 call :add_include imgui_bgfx\ imgui_bgfx\
 call :add_include sdl\include\ SDL\
 call :add_include spdlog\include\
 call :add_include flatbuffers\include\

 call :add_binaries build\bin\RelWithDebInfo\ "*.exp *.ilk *.pdb *.lib *.exe *.dll *.plist"
goto :eof


:add_include

 if not exist %~dp0\include (
  mkdir %~dp0\include
 )

  robocopy %~dp0%1 %~dp0\..\.vscode\include\%2 %~3 /S /NFL /NDL /NP /NS /NC /NJS /NJH

goto :eof

:add_binaries
  if not exist %~dp0\bin (
    mkdir %~dp0\bin
  )
 
  robocopy %~dp0%1 %~dp0\..\.vscode\lib\ %~2 /S /NFL /NDL /NP /NS /NC /NJS /NJH
goto :eof