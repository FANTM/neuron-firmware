PORT   := /dev/ttyACM0
RSHELL := rshell --buffer-size=512 -p $(PORT)
NAME   := pyboard

all: deploy test

deploy:
	$(RSHELL) rsync . /$(NAME); 
	$(RSHELL) repl $(NAME) machine.soft_reset\(\)~

test:
	cat $(PORT)
