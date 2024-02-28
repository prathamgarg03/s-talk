make:
	gcc main.c list.o sender.c receiver.c manager.c -lpthread -o s-talk

clean:
	rm s-talk