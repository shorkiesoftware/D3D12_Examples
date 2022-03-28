echo off
cls
set flags=-wd4201 -wd4389 -wd4018 -wd4100 -wd4505 -nologo -MT -GR- -EHa- -Oi 
REM set flags=-W4 -wd4201 -wd4389 -wd4018 -wd4100 -wd4505 -nologo -MT -GR- -EHa- -Oi -O2
IF %1 == system_build (
cl %flags% dx12_scratch.cpp /Z7 /volatile:iso /Fedx12_scratch /Fa /link -opt:ref -incremental:no user32.lib d3d12.lib dxgi.lib d3dcompiler.lib Xinput.lib Xaudio2.lib
)
IF %1 == application_run (
dx12_scratch
)