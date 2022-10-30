shell: cat_.out date_.out ls_.out mkdir_.out rm_.out
	gcc shell.c date_.c flags.c ls_.c mkdir_.c rm_.c tokeniser.c
	./a.out
	rm -f *.out *.o

cat_.out: cat_.c
	gcc cat_.c flags.c tokeniser.c -o cat_.out

date_.out: date_.c
	gcc date_.c flags.c tokeniser.c -o date_.out

ls_.out: ls_.c
	gcc ls_.c flags.c tokeniser.c -o ls_.out

mkdir_.out: mkdir_.c
	gcc mkdir_.c flags.c tokeniser.c -o mkdir_.out

rm_.out: rm_.c
	gcc rm_.c flags.c tokeniser.c -o rm_.out