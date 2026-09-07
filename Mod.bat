@echo off
setlocal

set "SRC_DIR=C:\Users\mr200\Documents\Unreal Projects\Palworld\Windows\Pal\Content\Paks"
set "DST_DIR=C:\Palworld\Palworld\Pal\Content\Paks\LogicMods"
set "NEW_NAME=LandmarkESP-PalWorldEdition_P"

if not exist "%DST_DIR%" (
    mkdir "%DST_DIR%"
)

for %%E in (pak utoc ucas) do (
    if exist "%SRC_DIR%\pakchunk810-Windows.%%E" (
        if exist "%DST_DIR%\%NEW_NAME%.%%E" (
            del /f /q "%DST_DIR%\%NEW_NAME%.%%E"
        )
        move /y "%SRC_DIR%\pakchunk810-Windows.%%E" "%DST_DIR%\%NEW_NAME%.%%E"
    ) else (
        echo [WARNING] Not found: %SRC_DIR%\pakchunk810-Windows.%%E
    )
)

echo Done.
exit