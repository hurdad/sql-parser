
CFLAGS = -std=c++11 -lstdc++ -Wall -Werror -I../src/ -L../

all:
	$(CXX) $(CFLAGS) example.cpp -o example -lsqlparser -ljsoncpp
	$(CXX) $(CFLAGS) json_example.cpp -o json_example -lsqlparser -ljsoncpp
