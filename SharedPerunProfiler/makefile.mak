CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -Werror -fPIC -O2
LDLIBS = 

SRC_FILES = $(wildcard *.cpp)
OBJ_FILES = $(patsubst %.cpp,%.o,$(SRC_FILES))

all: $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJ_FILES) -o PerunNetProfilerLin.so $(LDLIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o PerunNetProfilerLin.so