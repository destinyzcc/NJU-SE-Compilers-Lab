#! /bin/sh
for file in ./tests/Tests/*
do
    if test -f $file
    then
        echo $file >> normal.txt
        ./parser $file 2>> normal.txt
        echo "\n" >> normal.txt
    fi
done

for file in ./tests/At/*
do
    if test -f $file
    then
        echo $file >> A_output.txt
        ./parser $file 2>> A_output.txt
        echo "\n" >> A_output.txt
    fi
done

for file in ./tests/Bt/*
do
    if test -f $file
    then
        echo $file >> B_output.txt
        ./parser $file 2>> B_output.txt
        echo "\n" >> B_output.txt
    fi
done

for file in ./tests/Ct/*
do
    if test -f $file
    then
        echo $file >> C_output.txt
        ./parser $file 2>> C_output.txt
        echo "\n" >> C_output.txt
    fi
done

for file in ./tests/Dt/*
do
    if test -f $file
    then
        echo $file >> D_output.txt
        ./parser $file 2>> D_output.txt
        echo "\n" >> D_output.txt
    fi
done

