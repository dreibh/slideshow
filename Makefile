all:	slideshow2


converter.o:   converter.cc converter.h
	g++ converter.cc -c -o converter.o -Wall -g -O0 `Magick-config --cflags --cppflags`


slideshow2:	slideshow2.cc converter.o
	g++ slideshow2.cc -o slideshow2 converter.o -Wall -g -O0 `Magick-config --cflags --cppflags --ldflags --libs`

slideshow:	slideshow.cc
	g++ slideshow.cc -o slideshow -Wall -O2

getsize:	getsize.cc
	g++ getsize.cc -o getsize -Wall -O2

clean:
	rm -f slideshow slideshow2 getsize
