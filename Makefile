SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)  
PROGRAM = unit_test.out
CC = g++  

INCLUDE = 

LINKPARAM = -lgtest -lgtest_main -lpthread

CPPFLAGS = -ansi -O0 -w -g
													      
all: $(PROGRAM)  
	      
$(PROGRAM): $(OBJ)  
	$(CC) -o $(PROGRAM) $(OBJ) $(LINKPARAM)

.SUFFIXES : .cpp  
.cpp.o:  
	$(CC) $(INCLUDE) $(CPPFLAGS) -c $<  

%.o : %.cpp  
	$(CC) $(INCLUDE) $(CPPFLAGS) -c $<  

.PHONY : clean  
clean:  
	-rm -rf *.o  
	-rm -rf $(PROGRAM)
