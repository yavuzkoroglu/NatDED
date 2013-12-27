PRG = NatDED
SRC = $(wildcard *.c)
OBJ = $(patsubst %.c,%.o,$(SRC))
DAT = $(wildcard *.dat)
CC  = gcc

all:	$(PRG)
	$(info Making $(PRG))

$(PRG):	$(OBJ)
	$(info Linking...)
	@$(CC) $(OBJ) -o $(PRG)

$(OBJ):	$(SRC) $(DAT)
	$(info Compiling...)
	@$(CC) $(SRC) -c

clean:	
	@rm -f $(OBJ) $(PRG)
	
