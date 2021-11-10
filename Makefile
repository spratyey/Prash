.DEFAULT_GOAL := justmake

justmake:
	@ gcc -g globalheader.h pipes.h pipes.c pinfo.h pinfo.c bgproc.h bgproc.c ls.h ls.c pwdecho.h pwdecho.c cd.h cd.c commands.h commands.c shell.h shell.c -fsanitize=address -fsanitize=undefined -Wall -o prash

run:
	@ gcc -g globalheader.h pipes.h pipes.c pinfo.h pinfo.c bgproc.h bgproc.c ls.h ls.c pwdecho.h pwdecho.c cd.h cd.c commands.h commands.c shell.h shell.c -fsanitize=address -fsanitize=undefined -Wall -o prash
	@./prash