all: ImageMaker.exe
	mv $< ../..

ImageMaker.exe: ImageMaker.c
	gcc -o $@ $<

clean:
	rm -f ImageMaker.exe
