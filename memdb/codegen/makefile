CXX=gcc
CXXFLAGS=
LINK=g++
LINKFLAGS=
POSTFIX=
OBJ=.o
OUTPUT=-o 
LIB=-L/home/zsjin/code/trunk/server/lib -L/home/zsjin/code/thirdparty/stlport/lib
INC=-I/home/zsjin/code/trunk/server/inc/common -I/home/zsjin/code/thirdparty/stlport/inc
OBJECTS=codebuilder$(OBJ) codeflex$(OBJ) codebison$(OBJ)

rm = rm -rf

all : codebuilder$(POSTFIX)

codebuilder$(POSTFIX) : $(OBJECTS)
	$(LINK) $(LINKFLAGS) $(LIB) $(OBJECTS) -lcommon -lstlport -lpthread $(OUTPUT)codebuilder$(POSTFIX)

#libcodebuilder.a : $(OBJECTS)
#	ar r libcodebuilder.a $(OBJECTS)

codebuilder$(OBJ) : codebuilder.cpp
	g++ $(CXXFLAGS) $(INC) -c codebuilder.cpp $(OUTPUT)codebuilder$(OBJ)

#directory$(OBJ) : dir.cpp
#	g++ $(CXXFLAGS) $(INC) -c dir.cpp $(OUTPUT)directory$(OBJ)

codeflex$(OBJ) : codeflex.c
	$(CXX) $(CXXFLAGS) -c codeflex.c $(OUTPUT)codeflex$(OBJ)

codebison$(OBJ) : codebison.c
	$(CXX) $(CXXFLAGS) -c codebison.c $(OUTPUT)codebison$(OBJ)

codeflex.c : codeflex.l codebison.h
	flex -ocodeflex.c codeflex.l

codebison.c codebison.h : codebison.y 
	bison --output=codebison.c --defines=codebison.h codebison.y

.HPONY : cleanall cleanobj cleanexe
cleanall : cleanobj cleanexe
	$(rm) codebison.c codebison.h codeflex.c

cleanobj :
	$(rm) *$(OBJ)

cleanexe :
	$(rm) codebuilder$(POSTFIX)

