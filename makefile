#add all the files that you need to the files makefile variable below as a space seperated list
files = scene.cpp box.cpp texture_loader.cpp sun.cpp moon.cpp roof.cpp ribs.cpp dustbin.cpp roundChair.cpp highChairs.cpp plant.cpp cafeCounter.cpp coffeeMachine.cpp textured_mesh.cpp

main: main.cpp glad.c
	g++ -g shader.cpp $(files) main.cpp glad.c -lglfw -pthread -lGLEW -ldl -lGL -o main

clean:
	rm -f *.o main

run:
	./main

all:
	make clean
	make
	make run
