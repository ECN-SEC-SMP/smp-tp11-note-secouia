.PHONY: all run debug clean fclean re

all: build build/secouia

build:
	mkdir -p build

build/secouia: build/main.o build/Ticket.o build/Train.o build/Plateau.o build/Ville.o build/VoieFerree.o build/Joueur.o build/Partie.o
	g++ -Wall -Wextra -std=c++17 -o build/secouia build/main.o build/Ticket.o build/Train.o build/Plateau.o build/Ville.o build/VoieFerree.o build/Joueur.o build/Partie.o

build/main.o: main.cpp
	g++ -Wall -Wextra -std=c++17 -c main.cpp -o build/main.o

build/Ticket.o: Ticket.cpp
	g++ -Wall -Wextra -std=c++17 -c Ticket.cpp -o build/Ticket.o

build/Train.o: Train.cpp
	g++ -Wall -Wextra -std=c++17 -c Train.cpp -o build/Train.o

build/Plateau.o: Plateau.cpp
	g++ -Wall -Wextra -std=c++17 -c Plateau.cpp -o build/Plateau.o

build/Ville.o: Ville.cpp
	g++ -Wall -Wextra -std=c++17 -c Ville.cpp -o build/Ville.o

build/VoieFerree.o: VoieFerree.cpp
	g++ -Wall -Wextra -std=c++17 -c VoieFerree.cpp -o build/VoieFerree.o

build/Joueur.o: Joueur.cpp
	g++ -Wall -Wextra -std=c++17 -c Joueur.cpp -o build/Joueur.o

build/Partie.o: Partie.cpp
	g++ -Wall -Wextra -std=c++17 -c Partie.cpp -o build/Partie.o

run: all
	./build/secouia

debug: build
	g++ -Wall -Wextra -std=c++17 -g -O0 -c main.cpp -o build/main.o
	g++ -Wall -Wextra -std=c++17 -g -O0 -c Ticket.cpp -o build/Ticket.o
	g++ -Wall -Wextra -std=c++17 -g -O0 -c Train.cpp -o build/Train.o
	g++ -Wall -Wextra -std=c++17 -g -O0 -c Plateau.cpp -o build/Plateau.o
	g++ -Wall -Wextra -std=c++17 -g -O0 -c Ville.cpp -o build/Ville.o
	g++ -Wall -Wextra -std=c++17 -g -O0 -c VoieFerree.cpp -o build/VoieFerree.o
	g++ -Wall -Wextra -std=c++17 -g -O0 -c Joueur.cpp -o build/Joueur.o
	g++ -Wall -Wextra -std=c++17 -g -O0 -c Partie.cpp -o build/Partie.o
	g++ -Wall -Wextra -std=c++17 -g -O0 -o build/secouia build/main.o build/Ticket.o build/Train.o build/Plateau.o build/Ville.o build/VoieFerree.o build/Joueur.o build/Partie.o

clean:
	rm -f build/main.o build/Ticket.o build/Train.o build/Plateau.o build/Ville.o build/VoieFerree.o build/Joueur.o build/Partie.o

fclean: clean
	rm -rf build

re: fclean all
