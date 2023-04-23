###########################
#updated version
###########################

#declarations
CXX=g++
CXXFLAGS=-std=c++11 -Wall -Werror -pedantic -errors -DNDEBUG
CXXLINK=$(CXX)
OBJS=smash.o signals.o jobs.o commands.o
EXE=smash.exe
RM=rm -f

#rules


commands.o: commands.cpp commands.h
	$(CXX) $(CXXFLAGS) -c -fpic commands.cpp

jobs.o: jobs.cpp jobs.h
	$(CXX) $(CXXFLAGS) -c -fpic jobs.cpp

signals.o: signals.cpp signals.h
	$(CXX) $(CXXFLAGS) -c -fpic signals.cpp

$(EXE): $(OBJS)
	$(CXXLINK) smash.o -o $(EXE)



clean:
	$(RM) $(EXE) *.o *.exe
