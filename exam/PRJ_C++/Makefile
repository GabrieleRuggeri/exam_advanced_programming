MAIN = main.x
CXX = g++
CXXFLAGS = -Isrc -Wall -Wextra -std=c++14 
OPTIMIZATION = -O3


# eliminate default suffixes
.SUFFIXES:
SUFFIXES=

# just consider our own suffixes
.SUFFIXES: .cpp .x



all: $(MAIN) dox

.PHONY: all



dox: Doxygen/doxy.in
	doxygen $^

.PHONY: dox	

	

$(MAIN): main.cpp 
	$(CXX) $^ -o $(MAIN) $(CXXFLAGS) $(OPTIMIZATION)

clean:
	rm -rf *.x html latex

.PHONY: clean 
