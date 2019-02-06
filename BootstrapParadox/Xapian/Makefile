CC=g++
INCFLAG=-I/home/cc/opt/include
LIBFLAG=-L/home/cc/opt/lib
CFLAGS=-std=gnu++11
LIBS=-lxapian

all:
	$(CC) $(INCFLAG) $(LIBFLAG) $(CFLAGS) -o XSearchData_Meta.exe XSearchData_Meta.cpp $(LIBS)
	$(CC) $(INCFLAG) $(LIBFLAG) $(CFLAGS) -o XSearchData_Text.exe XSearchData_Text.cpp $(LIBS)
	

clean:
	rm *.exe
