#makefile

CXX=g++
CXXFLAGS=-std=c++11 -Wall -Werror -pedantic-errors -DNDEBUG -pthread
CXXLINK=$(CXX) $(CXXFLAGS)
OBJS=bank.o bankClass.o atm.o
EXE=Bank
RM=rm -f

Bank.exe: $(OBJS)
	$(CXXLINK) $(OBJS) -o Bank

bankClass.o: bankClass.cpp bankClass.h atm.h
	$(CXX) $(CXXFLAGS) -c bankClass.cpp -o bankClass.o

atm.o: atm.cpp atm.h bankClass.h
	$(CXX) $(CXXFLAGS) -c atm.cpp -o atm.o

clean:
	@$(RM) $(EXE) *.o *.exe