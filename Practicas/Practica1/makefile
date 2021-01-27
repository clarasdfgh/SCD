.SUFFIXES:
.PHONY: pc
.PRECIOUS: %_exe

compilador:=g++

t: test-semaforos_exe
	./$< | more

pc: prodcons_exe
	./$<

fu: fumadores_exe
	./$< 

%_exe: %.cpp Semaphore.cpp Semaphore.h
		$(compilador) -std=c++11 -pthread -o $@ $< Semaphore.cpp

clean:
	rm -f *_exe
