OBJECTS=./build/Editor.o ./build/EditorRow.o ./build/terminalState.o
FLAGS=-Wall -Wextra -pedantic
all: ${OBJECTS}
	g++ -o ./bin/main main.cpp ${FLAGS} ${OBJECTS}

./build/Editor.o:
	g++ ${FLAGS} ./Editor.cpp -c -o ./build/Editor.o
./build/EditorRow.o:
	g++ ${FLAGS} ./EditorRow.cpp -c -o ./build/EditorRow.o
./build/terminalState.o:
	g++ ${FLAGS} ./terminalState.cpp -c -o ./build/terminalState.o
clean:
	rm -r ./build/*
	rm -r ./bin/*
