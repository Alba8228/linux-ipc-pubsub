.PHONY: all server client1 client2 clean

all: server client1 client2

server:
	@cd server && $(MAKE)

client1:
	@cd client1 && $(MAKE)

client2:
	@cd client2 && $(MAKE)

clean:
	@cd server && $(MAKE) clean
	@cd client2 && $(MAKE) clean
	@cd client1 && $(MAKE) clean
	@cd common && rm -f *.o
	@echo "All clean."
