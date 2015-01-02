set srcfile1=dbtable
set srcfile2=dbloaddbplayerdata

.\codebuilder .\%srcfile1%.idl -s -I./dd/
.\codebuilder .\%srcfile2%.idl -s -I./dd/
pause