all:
	gcc cpu.c -o cpu -lrt -lpthread
	gcc hwa.c -o hwa -lrt -lpthread

clean:
	rm -f cpu hwa

