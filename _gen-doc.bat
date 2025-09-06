set UMBA_MDPP_EXTRA_OPTIONS="--batch-generate-page-index" "--batch-page-index-file=doc/Index.md"

set BRIEF_SCAN_PATHS="--scan=%~dp0doc\.."
rem set BRIEF_EXTRA_OPTS_TXT=--scan-notes "--notes-output-path=%~dp0\doc\_md"
set BRIEF_EXTRA_OPTS_MD=--scan-notes "--notes-output-path=%~dp0\doc\_md"
set BRIEF_EXTRA_EXCLUDE=--exclude-dir=-inc,-include


if exist "%~dp0\umba-brief-scanner.rsp" @set RSP="@%~dp0\umba-brief-scanner.rsp"
rem echo RSP="@%RSP%"

setlocal EnableDelayedExpansion
set BRIEF_SCAN_PATHS_UNQUOTED=!BRIEF_SCAN_PATHS:"=!
set BRIEF_EXTRA_OPTS_TXT_UNQUOTED=!BRIEF_EXTRA_OPTS_TXT:"=!
set BRIEF_EXTRA_OPTS_MD_UNQUOTED=!BRIEF_EXTRA_OPTS_MD:"=!
endlocal

set UBS_VERBOSE=--verbose=extra

if "%BRIEF_SCAN_PATHS_UNQUOTED%"=="" @set BRIEF_SCAN_PATHS="--scan=%~dp0doc\.."
rem if "%BRIEF_EXTRA_OPTS_TXT_UNQUOTED%"=="" @set BRIEF_EXTRA_OPTS_TXT=--scan-notes "--notes-output-path=%~dp0\doc/_md"
if "%BRIEF_EXTRA_OPTS_MD_UNQUOTED%"==""  @set BRIEF_EXTRA_OPTS_MD=--scan-notes "--notes-output-path=%~dp0\doc/_md"
umba-brief-scanner --where
rem umba-brief-scanner --overwrite %RSP%                %BRIEF_SCAN_PATHS% %BRIEF_EXTRA_OPTS_TXT% "%~dp0\doc/_md/_sources_brief.txt"
rem umba-brief-scanner --overwrite %RSP% --html         %BRIEF_SCAN_PATHS%                        "%~dp0\doc/_md/_sources_brief.html"
umba-brief-scanner %UBS_VERBOSE% --overwrite %RSP% --md --split-group %BRIEF_EXTRA_EXCLUDE% %BRIEF_SCAN_PATHS% %BRIEF_EXTRA_OPTS_MD%  "%~dp0\doc/_md/sources_brief.md_"
rem umba-brief-scanner %UBS_VERBOSE% --overwrite %RSP% --md --main-only    %BRIEF_SCAN_PATHS%                        "%~dp0\doc/_md/entry_points.md_"

rem exit 1

@set ARGLIST_OPTIONS=--arg-list-title=:Значение,:Описание --arg-list-type=table --arg-list-value-style=bold
@set VALLIST_OPTIONS=--val-list-title=:Значение,:Описание --val-list-type=table --val-list-value-style=bold

@set EXCLUDE_DIRS=--batch-exclude-dir=_libs,libs,_lib,lib,tests,test,rc,_generators,_enums,conf,_distr_conf,doxy,src,_src,.msvc2019,boost,icons
@set COMMON=--overwrite --copy-images --set-insert-options=proto-fmt=msdn
@rem 
@set GRAPHVIZ_OPTIONS=
@rem "--graphviz-output-path=%~dp0\doc\.generated_images"
@rem
@rem set TEMP_OPTS=--graphviz-keep-temp-dot-files
@set OPTIONS= %TEMP_OPTS% %COMMON% %VARS% %EXCLUDE_DIRS% %GRAPHVIZ_OPTIONS% %ARGLIST_OPTIONS% %VALLIST_OPTIONS% %UMBA_MDPP_EXTRA_OPTIONS% "--batch-scan-recurse=%~dp0\."
umba-md-pp %OPTIONS%

