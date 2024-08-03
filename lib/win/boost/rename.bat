@echo off
setlocal enabledelayedexpansion

REM 指定要重命名的目录
set "directory=.\"

REM 切换到指定目录
cd /d "%directory%"

REM 遍历所有匹配的文件
for /f "delims=" %%f in ('dir /b libboost_*.lib') do (
    REM 提取文件名和扩展名
    set "filename=%%~nf"
    set "extension=%%~xf"

    REM 删除指定的中间部分
    set "newname=!filename:-vc143-mt-x64-1_84=!!extension!"

    REM 执行重命名
    if not "!filename!!extension!"=="!newname!" (
        echo Renaming "%%f" to "!newname!"
        ren "%%f" "!newname!"
    )
)

REM 结束批处理
endlocal
pause
