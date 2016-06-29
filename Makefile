cc=g++
INC_DIR= ../../include
LIB_DIR= ../../lib
BIN_DIR= ../../bin

LDFLAG=-pthread -lgstreamer-0.10 -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lrt -lxml2 -lglib-2.0 -L/usr/X11R6/lib -L/usr/local/lib -L$(LIB_DIR)
LIBS= -lARgsub -lARvideo -lAR -lpthread -lglut -lGLU -lGL -lXi -lX11 -lm -lSDL -lSDL_mixer
CFLAG= -O -pthread -I/usr/include/gstreamer-0.10 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/libxml2 -I/usr/X11R6/include -I$(INC_DIR) -I/usr/include

OBJS =paint.o people.o hand.o startmenu.o mus_panel.o message.o button.o phone.o action.o talk_thread.o mode.o mymove.o star.o panel.o cdbox.o ait.o object.o world.o myshell.o videoavi.o myvim.o font.o bookshelf.o bmploader.o console.o mqoloader.o
HEADDERS =paint.h people.h hand.h startmenu.h mus_panel.h message.h button.h phone.h action.h talk_thread.h addresslist.h mode.h mymove.h star.h panel.h cdbox.h ait.h object.h world.h myshell.h myvim.h font.h bookshelf.h bmploader.h console.h mqoloader.h head.h 

all: $(BIN_DIR)/iffice

$(BIN_DIR)/iffice: iffice.o $(OBJS)
	g++ -o $(BIN_DIR)/iffice iffice.o $(OBJS) $(LDFLAG) $(LIBS) -laviplay

iffice.o: iffice.cpp $(HEADDERS)
	g++ -c $(CFLAG) iffice.cpp

console.o:console.cpp $(HEADDERS)
	g++ -c $(CFLAG) console.cpp

bmploader.o:bmploader.c $(HEADDERS)
	gcc -c $(CFLAG) bmploader.c

mqoloader.o:mqoloader.c $(HEADDERS)
	gcc -c $(CFLAG) mqoloader.c

bookshelf.o:bookshelf.cpp $(HEADDERS)
	g++ -c $(CFLAG) bookshelf.cpp

font.o:font.cpp $(HEADDERS)
	g++ -c $(CFLAG) font.cpp

myvim.o:myvim.cpp $(HEADDERS)
	g++ -c $(CFLAG) myvim.cpp

videoavi.o:videoavi.cpp videoavi.h
	g++ -c $(CFLAG) videoavi.cpp -laviplay

myshell.o:myshell.cpp $(HEADDERS)
	g++ -c $(CFLAG) myshell.cpp

world.o:world.cpp $(HEADDERS)
	g++ -c $(CFLAG) world.cpp

haibao.o:haibao.cpp $(HEADDERS)
	g++ -c $(CFLAG) haibao.cpp

object.o:object.cpp $(HEADDERS)
	g++ -c $(CFLAG) object.cpp

ait.o:ait.cpp $(HEADDERS)
	g++ -c $(CFLAG) ait.cpp

cdbox.o:cdbox.cpp $(HEADDERS)
	g++ -c $(CFLAG) cdbox.cpp

panel.o:panel.cpp $(HEADDERS)
	g++ -c $(CFLAG) panel.cpp

star.o:star.cpp $(HEADDERS)
	g++ -c $(CFLAG) star.cpp

mymove.o:mymove.cpp $(HEADDERS)
	g++ -c $(CFLAG) mymove.cpp

mode.o:mode.cpp $(HEADDERS)
	g++ -c $(CFLAG) mode.cpp

talk_thread.o:talk_thread.cpp $(HEADDERS)
	g++ -c $(CFLAG) talk_thread.cpp

action.o:action.cpp $(HEADDERS)
	g++ -c $(CFLAG) action.cpp

phone.o:phone.cpp $(HEADDERS)
	g++ -c $(CFLAG) phone.cpp

button.o:button.cpp $(HEADDERS)
	g++ -c $(CFLAG) button.cpp

message.o:message.cpp $(HEADDERS)
	g++ -c $(CFLAG) message.cpp

mus_panel.o:mus_panel.cpp $(HEADDERS)
	g++ -c $(CFLAG) mus_panel.cpp

startmenu.o:startmenu.cpp $(HEADDERS)
	g++ -c $(CFLAG) startmenu.cpp

hand.o:hand.cpp $(HEADDERS)
	g++ -c $(CFLAG) hand.cpp

people.o:people.cpp $(HEADDERS)
	g++ -c $(CFLAG) people.cpp

paint.o:paint.cpp $(HEADDERS)
	g++ -c $(CFLAG) paint.cpp

clean:
	rm -f *.o
	rm -f $(BIN_DIR)/iffice

allclean:
	rm -f *.o
	rm -f $(BIN_DIR)/iffice
