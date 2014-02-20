#CXX?=g++
LIBS=-lSDL2 -lSDL2_image -lGL -lGLU 
LIBS_DIR?=-L/home/tomas/Programming/SDL2/test/build/build/.libs -L/home/tomas/Programming/SDL2/SDL_image/.libs -I/home/tomas/Programming/SDL2/test/include -I/home/tomas/Programming/SDL2/SDL_image/
#CFLAGS?=-m32

shooter: main.o gl.o event.o level.o mouse.o kbd.o player.o tex.o logic.o mdl.o bot.o
	$(CXX) $(CFLAGS) main.o gl.o event.o level.o mouse.o player.o kbd.o tex.o logic.o mdl.o bot.o $(LIBS_DIR) $(LIBS) -o shooter
#`sdl-config --cflags --libs`
main.o: main.cpp shooter.h gl.h event.h level.h
	$(CXX) $(CFLAGS) $(LIBS_DIR) main.cpp -c

gl.o: gl.cpp gl.h shooter.h 
	$(CXX) $(CFLAGS) $(LIBS_DIR) gl.cpp -c

event.o: event.cpp event.h shooter.h gl.h
	$(CXX) $(CFLAGS) $(LIBS_DIR) event.cpp -c

level.o: level.cpp level.h shooter.h
	$(CXX) $(CFLAGS) $(LIBS_DIR) level.cpp -c

mouse.o: mouse.cpp mouse.h shooter.h
	$(CXX) $(CFLAGS) $(LIBS_DIR) mouse.cpp -c

kbd.o: kbd.cpp kbd.h shooter.h
	$(CXX) $(CFLAGS) $(LIBS_DIR) kbd.cpp -c

player.o: player.cpp player.h shooter.h
	$(CXX) $(CFLAGS) $(LIBS_DIR) player.cpp -c

tex.o: tex.cpp tex.h shooter.h
	$(CXX) $(CFLAGS) $(LIBS_DIR) tex.cpp -c

logic.o: logic.cpp logic.h shooter.h
	$(CXX) $(CFLAGS) $(LIBS_DIR) logic.cpp -c

bot.o: bot.cpp bot.h shooter.h
	$(CXX) $(CFLAGS) $(LIBS_DIR) bot.cpp -c

mdl.o: mdl/mdl.cpp mdl/mdl.h shooter.h
	$(CXX) $(CFLAGS) $(LIBS_DIR) mdl/mdl.cpp -c


clean:
	rm -f *.o shooter
