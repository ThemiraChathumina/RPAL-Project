# Define the C++ compiler to use
CXX = g++

# Define the C++ flags to use
CXXFLAGS = -Wall -Wextra -std=c++11

# Define the name of the executable
EXE = rpal20

# Define the source files
SRCS = test5.cpp cse.cpp st.cpp ast.cpp lexical_scanner.cpp

# Define the object files
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(EXE)

# Rule to build the executable
$(EXE): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule to build the object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

# Rule to clean up intermediate files
clean:
	del /f $(OBJS)

# Rule to clean up all generated files
distclean: clean
	del /f $(EXE)
