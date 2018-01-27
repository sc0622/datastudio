@echo off

title 为工程开发准备库环境

rem ****************************************
rem   variables
rem ****************************************

% 项目目录 %
set "ICD_DIR=%~dp0"

rem 设置环境变量

ver | find "4.0."  > NUL &&  goto set_env_wxp
ver | find "4.10." > NUL &&  goto set_env_wxp
ver | find "4.90." > NUL &&  goto set_env_wxp
ver | find "3.51." > NUL &&  goto set_env_wxp
ver | find "5.0."  > NUL &&  goto set_env_wxp
ver | find "5.1."  > NUL &&  goto set_env_wxp
ver | find "5.2."  > NUL &&  goto set_env_wxp
ver | find "6.0."  > NUL &&  goto set_env_win7_10
ver | find "6.1."  > NUL &&  goto set_env_win7_10
ver | find "6.2."  > NUL &&  goto set_env_win7_10
ver | find "6.3."  > NUL &&  goto set_env_win7_10
ver | find "10.0." > NUL &&  goto set_env_win7_10

:set_env_wxp

wmic ENVIRONMENT where "name='ICD_DIR'" delete
wmic ENVIRONMENT create name="ICD_DIR",username="<system>",VariableValue="%ICD_DIR%"

goto next

:set_env_win7_10

@setx ICD_DIR %ICD_DIR%

:next

goto exit

:pause
pause

:exit
exit /b 0
