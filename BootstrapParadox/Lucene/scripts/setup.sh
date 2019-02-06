#!/bin/bash

# check if the download directory exists, otherwise create it
if [ ! -d "download" ]
then
    mkdir download
fi

# check if the lucene source archive exists, otherwise download it
if [ ! -e "download/lucene-7.1.0-src.tgz" ]
then
    cd download
    wget https://archive.apache.org/dist/lucene/java/7.1.0/lucene-7.1.0-src.tgz
    cd ..
fi

# check if the lib directory exists, otherwise create it
if [ ! -d "lib" ]
then
    mkdir lib
fi

# check if the lucene source lib exists, otherwise extract it
if [ ! -d "lib/lucene-7.1.0" ]
then
    tar xzvf download/lucene-7.1.0-src.tgz -C lib/
fi

# check if java 8 openjdk exists, other install it
dpkg -s openjdk-8-jdk &> /dev/null
if [ $? -eq 1 ]
then
    sudo apt install -y openjdk-8-jdk
fi

# check if and build tool exists, otherwise install it
dpkg -s ant &> /dev/null
if [ $? -eq 1 ]
then
    sudo apt install -y ant
fi

# check if ivy build tool exists, otherwise install it
dpkg -s ivy &> /dev/null
if [ $? -eq 1 ]
then
    sudo apt install -y ivy
fi

# check if lucene jar exists, otherwise build it
if [ ! -e "lib/lucene-7.1.0/build/core/lucene-core-7.1.0-SNAPSHOT.jar" ]
then
    if [ ! -d "~/.ant/lib" ]
    then
        mkdir -p ~/.ant/lib
    fi
    
    cd lib/lucene-7.1.0
    ant ivy-bootstrap
    ant
    cd ../..
fi
