JC=javac
CP=lib/lucene-7.1.0/core/src/java:lib/lucene-7.1.0/queryparser/src/java -d bin

all:
	mkdir -p bin
	$(JC) -cp $(CP) src/XSearchData.java
	$(JC) -cp $(CP) src/testMultiMeta.java
    
clean:
	rm bin/*.class
