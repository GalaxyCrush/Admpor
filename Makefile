OBJECTOS = memory.o process.o main.o client.o enterprise.o intermediary.o apsignal.o aptime.o configuration.o log.o stats.o synchronization.o

process.o = process.h memory.h main.h client.h enterprise.h intermediary.h synchronization.h
memory.o = memory.h main.h
main.o =  memory.h process.h main.h synchronization.h apsignal.h aptime.h stats.h log.h configuration.h
enterprise.o = enterprise.h main.h memory.h synchronization.h aptime.h apsignal.h
client.o = main.h memory.h client.h synchronization.h aptime.h apsignal.h
intermediary.o = main.h memory.h intermediary.h synchronization.h aptime.h apsignal.h
apsignal.o = apsignal.h main.h synchronization.h memory.h 
aptime.o = aptime.h
configuration.o = configuration.h main.h
log.o = log.h main.h
stats.o = stats.h main.h
synchronization.o = synchronization.h

OBJ_DIR = obj
INC = -I include
EX = bin
CC = gcc
LM = -lm


vpath %.o $(OBJ_DIR)

out: $(OBJECTOS)
	$(CC) $(addprefix $(OBJ_DIR)/,$(OBJECTOS)) -o $(EX)/admpor $(LM)

%.o: src/%.c $($@)
	$(CC) $(INC) -o $(OBJ_DIR)/$@ -c $< $(LM)

clean:
	rm -rf $(OBJ_DIR)/*.o $(EX)/admpor $(EX)/log.txt $(EX)/stats.txt
