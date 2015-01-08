dump: dump.cpp
	clang++ -std=c++11 -Wall -Werror -ggdb3 -O0 -lclang dump.cpp -o dump
