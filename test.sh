#!/bin/sh

if [ ! -d "test" ]; then 
	mkdir "test" 
fi

cd $(dirname $0)/test

if [ ! -f "test.txt" ]; then
	echo "hello world" > test.txt
else
	rm -rf test.txt
	echo "hello world" > test.txt
fi

../your_program.sh init

../build/Debug/git cat-file -p 95d09f2b10159347eece71399a7e2e907ea3df4f
