# Compiler
CXX = clang++

# Compiler flags
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Executable name
TARGET = game

# Source files
SRCS = game.cpp map.cpp base.cpp fighter.cpp utils.cpp

# Object files (replace .cpp with .o)
OBJS = $(SRCS:.cpp=.o)

# Header files (dependencies)
HEADERS = map.h base.h fighter.h utils.h

# Default target
all: $(TARGET)

# Link the object files to create the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Compile the source files into object files
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up generated files
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean
