@echo off
rem bat to delete GNU objects and intermediate files
if not exist ..\Docu md ..\Docu
set app_path="%PATH_GNU_CORTEX%\bin"
set app_source="..\objects\IEC60335_ClassB.elf"
%app_path%\arm-hitex-elf-readelf -S %app_source%   > ..\Docu\info_readelf.txt
%app_path%\arm-hitex-elf-strings %app_source%      > ..\Docu\info_string.txt
%app_path%\arm-hitex-elf-size %app_source%         > ..\Docu\info_size.txt
%app_path%\arm-hitex-elf-objdump -h %app_source%   > ..\Docu\info_objects.txt
%app_path%\arm-hitex-elf-nm -a -l %app_source%     > ..\Docu\info_nm.txt
set app_path=
set app_source=
