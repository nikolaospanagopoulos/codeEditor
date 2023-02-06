OBJECTS=./build/Editor.o
FLAGS=-Wall -Wextra -pedantic
all: ${OBJECTS}
	g++ -o ./bin/main main.cpp ${FLAGS} ${OBJECTS}

./build/Editor.o:
	g++ ${FLAGS} ./Editor.cpp -c -o ./build/Editor.o
clean:
	rm -r ./build/*
	rm -r ./bin/*
