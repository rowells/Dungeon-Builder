INCLUDE = -I/usr/include/
LIBDIR  = -L/usr/X11R6/lib 

COMPILERFLAGS = -Wall
CC = gcc
CFLAGS = $(COMPILERFLAGS) $(INCLUDE)
LIBRARIES = -lX11 -lglut -lGL -lGLU -lm 


all: $(basename $(wildcard dungeonBuilder.c))

dist: $(foreach file,$(basename $(wildcard dungeonBuilder.c)),$(file).tar.gz)


dungeonBuilder.tar.gz : 
	tar cvf $(subst .tar.gz,.tar,$@) README makefile $(subst .tar.gz,.c,$@) $(wildcard Data/$(subst .tar.gz,,$@)/*); \
	gzip $(subst .tar.gz,.tar,$@);


dungeonBuilder : dungeonBuilder.o
	$(CC) $(CFLAGS) -o $@ $(LIBDIR) $< $(LIBRARIES)  


clean:
	rm $(wildcard dungeonBuilder)


distclean:
	rm $(wildcard dungeonBuilder.tar.gz)
	


