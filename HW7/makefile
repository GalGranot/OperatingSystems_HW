#makefile

CXX=g++
CXXFLAGS=-std=c++11 -Wall -Werror -pedantic-errors -DNDEBUG -pthread
CXXLINK=$(CXX) $(CXXFLAGS)
OBJS=ttftps.o
EXE=ttftps
RM=rm -f
	
ttftps.exe: $(OBJS)
	$(CXX) $(CXXFLAGS) -c ttftps.cpp -o ttftps.o

clean:
	@$(RM) $(EXE) *.o *.exe	
