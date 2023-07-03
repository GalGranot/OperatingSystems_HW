#makefile

CXX=g++
CXXFLAGS=-std=c++11 -Wall -Werror -pedantic-errors -DNDEBUG 
CXXLINK=$(CXX) $(CXXFLAGS)
OBJS=ttftps.o
EXE=ttftps
RM=rm -f
	
ttftps.exe: $(OBJS)
	$(CXX) $(CXXFLAGS) ttftps.o -o ttftps
	
ttftps.o: ttftps.cpp
	$(CXX) $(CXXFLAGS) -c *.cpp -o ttftps.o



clean:
	@$(RM) $(EXE) *.o *.exe	
