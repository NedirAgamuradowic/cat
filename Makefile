CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c11 -pedantic
TARGET = s21_cat
SRC = s21_cat.c 
OBJ = $(SRC: .c = .o)
RM = rm -f
INTEGRATION = s21_cat.sh

all:$(TARGET)

$(TARGET):$(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

%.o:%.c
	$(CC) $(CFLAGS)  -c $< -o $@

test: $(INTEGRATION)
	bash $(INTEGRATION)

clean:
	$(RM) $(TARGET)
