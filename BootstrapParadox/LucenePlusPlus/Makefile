CC=g++
INCFLAG=-Ilib/luceneplusplus-3.0.7/include/lucene++
LIBFLAG=-Llib/luceneplusplus-3.0.7/lib
CFLAGS=-std=gnu++11
LIBS=-lpthread -lboost_system -llucene++ -llucene++-contrib

all:
	mkdir -p bin
	$(CC) $(INCFLAG) $(LIBFLAG) $(CFLAGS) -o bin/lpp_meta.exe \
		src/lpp_meta.cpp $(LIBS)
    $(CC) $(INCFLAG) $(LIBFLAG) $(CFLAGS) -o bin/lpp_search.exe \
		src/lpp_search.cpp $(LIBS)

clean:
	$(RM) -r bin/*.exe
