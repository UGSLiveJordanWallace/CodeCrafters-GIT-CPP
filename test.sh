#!/bin/sh

if [ ! -d "test" ]; then 
	mkdir "test" 
else
	rm -rf "test"
	mkdir "test"
fi

cd $(dirname $0)/test

if [ ! -f "test.txt" ]; then
	echo "hello world" > "test.txt"
fi
if [ ! -f "test_file_0.txt" ]; then
	echo "hello world test_0" > "test_file_0.txt"
fi

if [ ! -d "test_dir_1" ]; then
	mkdir "test_dir_1"
	echo "hello world test_1" > "test_dir_1/test_file_1.txt"
fi
if [ ! -d "test_dir_2" ]; then
	mkdir "test_dir_2"
	echo "hello world test_2" > "test_dir_2/test_file_2.txt"
fi
if [ ! -d "test_dir_3" ]; then
	mkdir "test_dir_3"
	echo "hello world test_3" > "test_dir_3/test_file_3.txt"
fi

../your_program.sh init
../build/Debug/git write-tree
