all:	slideshow getsize slideshow2

slideshow2:	slideshow2.cc
	g++ slideshow2.cc -o slideshow2 -Wall -g -O0

slideshow:	slideshow.cc
	g++ slideshow.cc -o slideshow -Wall -O2

getsize:	getsize.cc
	g++ getsize.cc -o getsize -Wall -O2

clean:
	rm -f slideshow slideshow2 getsize
