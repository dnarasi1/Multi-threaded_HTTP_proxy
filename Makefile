all: proxy.c
	gcc -pthread -o proxy proxy.c
clean:
	rm proxy
