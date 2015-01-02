set srcfile1=dbtable
set srcfile2=dbloaddbplayerdata

.\codebuilder_back .\%srcfile1%.idl -s -I./dd/
.\codebuilder_back .\%srcfile2%.idl -s -I./dd/
pause