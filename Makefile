build: source

source: source.c functions.c parse.c
	gcc -Wall -Wextra source.c functions.c parse.c -lm -o image_editor
	
clean:
	rm -rf image_editor
