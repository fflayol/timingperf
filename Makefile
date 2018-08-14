default:test
test:
	g++   ex1.cpp -std=c++11 -o ex1.out
install:
	cp performancetiming.hpp /usr/local/include
