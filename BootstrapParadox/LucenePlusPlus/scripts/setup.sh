#!/bin/bash

# check if the download directory exists, otherwise create it
if [ ! -d "download" ]
then
    mkdir download
fi

# check if the LucenePlusPlus directory exists, otherwise clone it
if [ ! -e "download/LucenePlusPlus" ]
then
    cd download
    git clone https://github.com/luceneplusplus/LucenePlusPlus.git
    cd ..
fi

# check if the LucenePlusPlus lib directory exists, otherwise create it
if [ ! -d "lib" ]
then
    mkdir lib
fi

# check if gcc (C compiler) exists, otherwise install it
dpkg -s gcc &> /dev/null
if [ $? -eq 1 ]
then
    sudo apt install -y gcc
fi

# check if g++ (C++ compiler) exists, otherwise install it
dpkg -s g++ &> /dev/null
if [ $? -eq 1 ]
then
    sudo apt install -y g++
fi

# check if cmake exists, otherwise install it
dpkg -s cmake &> /dev/null
if [ $? -eq 1 ]
then
    sudo apt install -y cmake
fi

# check if gcc exists, otherwise install it
dpkg -s libboost-date-time-dev &> /dev/null
if [ $? -eq 1 ]
then
    sudo apt install -y libboost-date-time-dev
fi

# check if libboost-filesystem-dev exists, otherwise install it
dpkg -s libboost-filesystem-dev &> /dev/null
if [ $? -eq 1 ]
then
    sudo apt install -y libboost-filesystem-dev
fi

# check if libboost-regex-dev exists, otherwise install it
dpkg -s libboost-regex-dev &> /dev/null
if [ $? -eq 1 ]
then
    sudo apt install -y libboost-regex-dev
fi

# check if libboost-thread-dev exists, otherwise install it
dpkg -s libboost-thread-dev &> /dev/null
if [ $? -eq 1 ]
then
    sudo apt install -y libboost-thread-dev
fi

# check if libboost-iostreams-dev exists, otherwise install it
dpkg -s libboost-iostreams-dev &> /dev/null
if [ $? -eq 1 ]
then
    sudo apt install -y libboost-iostreams-dev
fi

cd download/LucenePlusPlus
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=../../../lib/luceneplusplus-3.0.7 ..
make
make install
cd ../..
