SRC:= $(wildcard renderer/src/*.cpp)
OBJS:= $(patsubst renderer/src/%.cpp, renderer/objs/%.o, $(SRC))
FLAGS:= -fPIC -c -I include

CXX=g++
CXXFLAGS=$(FLAGS)

renderer/objs/%.o: renderer/src/%.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@ 

all: $(OBJS)
	ar rcs lib/librenderer.a $(OBJS)
	g++ -I include -L lib src/main.cpp -o main.exe -lmingw32 -lSDL3 -lrenderer