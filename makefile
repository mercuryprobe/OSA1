shell: cat_.out date_.out ls_.out mkdir_.out rm_.out
	#https://stackoverflow.com/questions/73435637/how-can-i-fix-usr-bin-ld-warning-trap-o-missing-note-gnu-stack-section-imp
	gcc shell.c tokeniser.c -z noexecstack
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