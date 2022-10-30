shell: cat_.out
	gcc shell.c date_.c flags.c ls_.c mkdir_.c rm_.c tokeniser.c
	./a.out
	

cat_.out: cat_.c
	gcc cat_.c flags.c tokeniser.c -o cat_.out
