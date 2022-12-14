@echo off

if NOT DEFINED proj_root (
call "%~dp0\shell.bat"
)
pushd "%proj_root%"

IF NOT EXIST ".\build" ( mkdir ".\build")

SETLOCAL

pushd ".\build"

set freetype_dir=%proj_root%\libs\freetype

set      ignored_warnings=-wd4201 -wd4100 -wd4189 -wd4456 -wd4505
set       windows_h_flags=-DNOMINMAX -DWIN32_LEAN_AND_MEAN
set          flag_defines=-DDS_INTERNAL=1 -DDS_DEBUG=1
set common_compiler_flags=-DUNICODE=1 -diagnostics:column -MTd -nologo -Gm- -GR- -EHa- -Od -Oi -WX -W4 %ignored_warnings% -FAsc -Z7 %flag_defines% %windows_h_flags% -I%freetype_dir%\include
set   common_linker_flags=-incremental:no -opt:ref user32.lib gdi32.lib %freetype_dir%\static\freetype.lib

REM 64-bit build
del *.pdb > NUL 2> NUL
set   code_root=%proj_root%\code
set source_list="%code_root%\win32_layer.cpp"
cl %common_compiler_flags% %source_list% -Fmwin32_layer.map /link %common_linker_flags%

popd REM .\build
popd REM %proj_root
ENDLOCAL
