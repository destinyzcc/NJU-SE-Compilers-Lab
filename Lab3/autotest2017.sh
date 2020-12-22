#! /bin/sh

for file in ./Test/*
do
    if test -f $file
    then
        echo $file >> test2017.txt
        ./parser $file 2>> test2017.txt
        echo "\n" >> test2017.txt
    fi
done

