MF=     Makefile

CC=     mpicc

CFLAGS= -O3
LFLAGS= $(CFLAGS) -lm

EXE=    hybrid_mpiACC

INC= 


SRC= \
        hybrid_mpiACC.c \

#
# No need to edit below this line
#

.SUFFIXES:
.SUFFIXES: .c .o

OBJ=    $(SRC:.c=.o)

.c.o:
		$(CC) $(CFLAGS) -c $<

all:    $(EXE)

$(OBJ): $(INC)

$(EXE): $(OBJ)
		$(CC) $(CFLAGS) -o $@ $(OBJ) $(LFLAGS)

$(OBJ): $(MF)

clean:
		rm -f $(OBJ) $(EXE) core                             
