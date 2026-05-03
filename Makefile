NAME = pseudo-shell

CFLAGS = -Wall -Wextra

SRCS = main.c command.c string_parser.c
OBJS = main.o command.o string_parser.o
OUT = output.txt

RM = rm -f
RMDIR = rm -rf

all: $(NAME)
$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

clean:
	$(RM) $(OBJS)
	$(RM) $(NAME)
	$(RM) $(OUT)