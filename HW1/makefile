
#new file
CXX=g++
CXXFLAGS=-std=c++11 -Wall -Werror -pedantic -errors -DNDEBUG
CXXLINK=$(CXX)
OBJS=smash.o signals.o jobs.o commands.o
EXE=smash.exe
RM=rm -f

smash.exe: $(OBJS)
	$(CXXLINK) $(OBJS) -o smash.EXE

commands.o: commands.cpp commands.h jobs.h
	$(CXX) $(CXXFLAGS) -c commands.cpp -o commands.o

jobs.o: jobs.cpp jobs.h
	$(CXX) $(CXXFLAGS) -c jobs.cpp -o jobs.o

signals.o: signals.cpp signals.h
	$(CXX) $(CXXFLAGS) -c signals.cpp -o signals.o

clean:
	@$(RM) $(EXE) *.o *.exe