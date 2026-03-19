.PHONY: all run debug clean fclean re

all: build build/secouia

build:
    mkdir -p build

build/secouia: build/main.o build/Ticket.o build/Train.o
    g++ -Wall -Wextra -std=c++17 -o build/secouia build/main.o build/Ticket.o build/Train.o

build/main.o: main.cpp
    g++ -Wall -Wextra -std=c++17 -c main.cpp -o build/main.o

build/Ticket.o: Ticket.cpp
    g++ -Wall -Wextra -std=c++17 -c Ticket.cpp -o build/Ticket.o

build/Train.o: Train.cpp
    g++ -Wall -Wextra -std=c++17 -c Train.cpp -o build/Train.o

run: all
    ./build/secouia

debug: build
    g++ -Wall -Wextra -std=c++17 -g -O0 -c main.cpp -o build/main.o
    g++ -Wall -Wextra -std=c++17 -g -O0 -c Ticket.cpp -o build/Ticket.o
    g++ -Wall -Wextra -std=c++17 -g -O0 -c Train.cpp -o build/Train.o
    g++ -Wall -Wextra -std=c++17 -g -O0 -o build/secouia build/main.o build/Ticket.o build/Train.o

clean:
    rm -f build/main.o build/Ticket.o build/Train.o

fclean: clean
    rm -rf build

re: fclean all