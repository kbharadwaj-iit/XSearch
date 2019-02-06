#!/bin/bash

# this function clear the OS IO cache and bufers
function clear_cache {
	sync
	sudo sh -c 'echo 3 > /proc/sys/vm/drop_caches'
}

export CLASSPATH="lib/lucene-7.1.0/build/core/classes/java/:bin"

datentime=$(date +'%Y-%m-%d-%H:%M')

# fill in the directory path that contains the input files
input_path="../Data/"

log="iteration-$datentime.log"

echo -n "" > $log

# run the test five times
for i in {1..5}
do
    input_file="input.dat"
    terms_file="terms.dat"

    clear_cache
    java XSearchData $input_path$input_file $input_path$terms_file &>> $log
done

