OUTPUT = ./output
SRC = $(wildcard *.c)
OBJ = $(patsubst %.c, $(OUTPUT)/%.o, $(SRC))
RM =  ~/tools/trash.sh

all: service client

service:service.o wrap.o
	gcc $(addprefix $(OUTPUT)/, $^) -o $(OUTPUT)/$@ -lpthread

client:client.o wrap.o
	gcc $(addprefix $(OUTPUT)/, $^) -o $(OUTPUT)/$@

%.o:%.c
	gcc -c $< -o $(OUTPUT)/$@ -Wall

.PHONY:
cleanall:
	$(RM) $(OBJ) $(OUTPUT)/service $(OUTPUT)/client