exec = grade.out
sources = $(wildcard *.c)
objects = $(sources:.c=.o)
flags = -g


$(exec): $(objects)
	gcc $(objects) $(flags) -o $(exec)

%.o: %.c headers/%.h
	gcc -c $(flags) $< -o $@

install:
	make
	cp ./grade.out /usr/local/bin/grade
	make clean

clean:
	-rm *.out
	-rm *.o