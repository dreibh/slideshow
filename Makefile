all:	slideshow getsize

slideshow:	slideshow.cc
	g++ slideshow.cc -o slideshow -Wall -O2

getsize:	getsize.cc
	g++ getsize.cc -o getsize -Wall -O2

clean:
	rm -f slideshow getsize

