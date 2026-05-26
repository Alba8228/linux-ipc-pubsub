COMMON_DIR = ../common

OBJS = $(COMMON_DIR)/sem.o $(COMMON_DIR)/shmfifo.o $(COMMON_DIR)/shm.o $(COMMON_DIR)/msg.o $(COMMON_DIR)/config.o $(COMMON_DIR)/net.o server.o main.o thread.o hashmap.o
server:$(OBJS)
	@gcc $^ -o $@ -lpthread
	@echo "Done."

%.o : %.c
	@gcc -c $< -o $@ -I$(COMMON_DIR)

$(COMMON_DIR)/%.o: $(COMMON_DIR)/%.c
	@gcc -c $< -o $@ -I$(COMMON_DIR)

clean:
	@rm -f *.o server
	@echo "Clean."
