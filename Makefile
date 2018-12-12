objects = png.o game.o log.o main.o gldefs.o shader.o m3d.o

all: frame $(objects)

frame: $(objects)
	gcc  -Og -W $(objects) -o frame -lpng -lopengl32 -mwindows -fwhole-program -ggdb

$(objects): %.o: %.c
	gcc -Iinclude/ -Og -W -Wall $^ -o $@ -c -flto

.PHONY: clean

clean:
	rm -f $(objects)
	rm -f frame