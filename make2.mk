#
CC=gcc

CFLAGS=-Wall -pedantic -g

LIBS=-lncurses

SRCS=GameEngine.c

OBJS=$(SRCS:.c=.o)

NAME=GameEngine

.PHONY: depend clean

all: $(NAME)
	python3 translator.py mygame.drag

	@echo  $(NAME) has compiled.

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(NAME) $(OBJS) $(LFLAGS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ $(NAME)

depend: $(SRCS)
	makedepend $(INCLUDES) $^

#
