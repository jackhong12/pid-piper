CC := g++
CCFLAG := -g -lrt
SRC := posix_shm.cpp

all:
	g++ $(SRC) -o attack -DATTACK_SIGNAL_GENERATOR $(CCFLAG)
	g++ $(SRC) -o log -DLOG_DATA $(CCFLAG)

clean:
	rm -f attack

