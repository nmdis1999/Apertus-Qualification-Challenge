all: code

code: code.o lodepng.o
       g++ code.o lodepng.o -o code

code.o: code.cpp
         g++ -c code.cpp

lodepng.o: lodepng.cpp
         g++ -c lodepng.cpp

clean:
         rm -rf *o code

     

