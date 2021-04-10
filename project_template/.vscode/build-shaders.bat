@echo off
cls
set "SHADERC=%~dp0\lib\shaderc.exe"

:main
    pushd .\shaders
    call :compile_shaders vulkan
    call :compile_shaders opengl
    call :compile_shaders dx9
    call :compile_shaders dx11
    call :compile_shaders dx12
    popd

goto :eof

:compile_shaders
  setlocal enabledelayedexpansion
  
  if not exist gen ( 
      mkdir gen 
  )

  if [%1]==[opengl] (
    set "BGFX_PROFILE_VS=--type vertex --platform windows --profile 120"
    set "BGFX_PROFILE_FS=--type fragment --platform windows --profile 120"
    set "BGFX_PROFILE_CS=--type compute --platform windows --profile 430"
  )

  if [%1]==[vulkan] (  
    set "BGFX_PROFILE_VS=--type vertex --platform linux --profile spirv"
    set "BGFX_PROFILE_FS=--type fragment --platform linux --profile spirv"
    set "BGFX_PROFILE_CS=--type compute --platform linux --profile spirv"
  )

  if [%1]==[dx9] (  
    set "BGFX_PROFILE_VS=--type vertex --platform windows --profile vs_3_0 -O 3"
    set "BGFX_PROFILE_FS=--type fragment --platform windows --profile ps_3_0 -O 3"
    set "BGFX_PROFILE_CS=--type compute --platform windows --profile cs_5_0 -O 3"
  )

  if [%1]==[dx11] (  
    set "BGFX_PROFILE_VS=--type vertex --platform windows --profile vs_5_0 -O 3"
    set "BGFX_PROFILE_FS=--type fragment --platform windows --profile ps_5_0 -O 3"
    set "BGFX_PROFILE_CS=--type compute --platform windows --profile cs_5_0 -O 3"
  )

  if [%1]==[dx12] (  
    set "BGFX_PROFILE_VS=--type vertex --platform windows --profile vs_5_0 -O 3"
    set "BGFX_PROFILE_FS=--type fragment --platform windows --profile ps_5_0 -O 3"
    set "BGFX_PROFILE_CS=--type compute --platform windows --profile cs_5_0 -O 3"
  )

  if [%1]==[metal] (  
    set "BGFX_PROFILE_VS=--type vertex --platform osx --profile metal"
    set "BGFX_PROFILE_FS=--type fragment --platform osx --profile metal"
    set "BGFX_PROFILE_CS=--type compute --platform osx --profile metal"
  )

  if [%1]==[pssl] (  
    set "BGFX_PROFILE_VS=--type vertex --platform orbis --profile pssl"
    set "BGFX_PROFILE_FS=--type fragment --platform orbis --profile pssl"
    set "BGFX_PROFILE_CS=--type compute --platform orbis --profile pssl"
  )

  if [%1]==[android] (  
    set "BGFX_PROFILE_VS=--type vertex --platform android"
    set "BGFX_PROFILE_FS=--type fragment --platform android"
    set "BGFX_PROFILE_CS=--type compute --platform android"
  )

  if [%1]==[nacl] (  
    set "BGFX_PROFILE_VS=--type vertex --platform nacl"
    set "BGFX_PROFILE_FS=--type fragment --platform nacl"
    set "BGFX_PROFILE_CS=--type compute --platform android"
  )
  

  for /R .\ %%f in (*.vs) do (
    set B=%%f
    call %SHADERC% -f !B:%CD%\=! -o gen\%%~nxf.%1 %BGFX_PROFILE_VS% --bin2c
  )

  for /R .\ %%f in (*.fs) do (
    set B=%%f
    call %SHADERC% -f !B:%CD%\=! -o gen\%%~nxf.%1 %BGFX_PROFILE_FS% --bin2c
  )

  for /R .\ %%f in (*.ps) do (
    set B=%%f
    call %SHADERC% -f !B:%CD%\=! -o gen\%%~nxf.%1 %BGFX_PROFILE_CS% --bin2c
  )

goto :eof