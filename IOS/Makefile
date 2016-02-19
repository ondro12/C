CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic 
LFLAGS = -lpthread -lrt

rivercrossing : rivercrossing.c
	gcc $(CFLAGS) -o $@ $< $(LFLAGS)

clean :
	rm -f rivercrossing
