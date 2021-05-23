@echo off

:main
 %1
 call :add_include bgfx\include\
 call :add_include bimg\include\
 call :add_include bx\include\
 call :add_include bx\include\compat\msvc\
 call :add_include enet\include\
 call :add_include flecs\ flecs\ "flecs.c flecs.h /LEV:1"
 call :add_include gameframe\ gameframe\
 call :add_include glm\glm\ glm\
 call :add_include imgui\ imgui\ "*.h *.hpp /XD examples"
 call :add_include imgui_bgfx\ imgui_bgfx\
 call :add_include sdl\include\ SDL\
 call :add_include spdlog\include\
 call :add_include glad\ glad\
 call :add_include flatbuffers\include\
 call :add_include bullet3\src\ bullet\ "*.h *.hpp "
 call :add_include raylib\src\ raylib\ "*.h *.hpp "


 call :add_binaries build\bin\MinSizeRel\ "*.exp *.ilk *.pdb *.lib *.exe *.dll *.plist"
 call :move_dlls build\bin\MinSizeRel\ "*.dll"
goto :eof


:add_include
  robocopy %~dp0%1 %~dp0\..\.vscode\include\%2 %~3 /S /NFL /NDL /NP /NS /NC /NJS /NJH
goto :eof

:add_binaries 
  robocopy %~dp0%1 %~dp0\..\.vscode\lib\ %~2 /S /NFL /NDL /NP /NS /NC /NJS /NJH
goto :eof

:move_dlls 
  robocopy %~dp0%1 %~dp0\..\.vscode\bin\ %~2 /S /NFL /NDL /NP /NS /NC /NJS /NJH
goto :eof