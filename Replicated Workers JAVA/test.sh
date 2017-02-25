#!/bin/bash

function testoutput {
	DIFF=$(diff -b test$1/PRIME.out PRIME.out) 
	if [ "$DIFF" != "" ] 
	then
		echo "Test $1 failed for PRIME.out"
		exit
	fi
	
	DIFF=$(diff -b test$1/FIB.out FIB.out) 
	if [ "$DIFF" != "" ] 
	then
		echo "Test $1 failed for FIB.out"
		exit
	fi
	
	DIFF=$(diff -b test$1/SQUARE.out SQUARE.out) 
	if [ "$DIFF" != "" ] 
	then
		echo "Test $1 failed for SQUARE.out"
		exit
	fi
	
	DIFF=$(diff -b test$1/FACT.out FACT.out) 
	if [ "$DIFF" != "" ] 
	then
		echo "Test $1 failed for FACT.out"
		exit
	fi
}

echo "Running test 1"
java -jar eventqueue.jar 10 5 test1/file1.in test1/file2.in
testoutput 1
echo "Test 1 completed successfully"

echo "Running test 2"
java -jar eventqueue.jar 20 10 test2/file1.in test2/file2.in test2/file3.in test2/file4.in
testoutput 2
echo "Test 2 completed successfully"

echo "Running test 3"
java -jar eventqueue.jar 50 50 test3/file1.in test3/file2.in test3/file3.in test3/file4.in
testoutput 3
echo "Test 3 completed successfully"

echo "Running test 4"
java -jar eventqueue.jar 100 50 test4/file1.in test4/file2.in test4/file3.in test4/file4.in
testoutput 4
echo "Test 4 completed successfully"

echo "Running test 5"
java -jar eventqueue.jar 100 100 test5/file1.in test5/file2.in test5/file3.in test5/file4.in
testoutput 5
echo "Test 5 completed successfully"

echo "Running test 6"
java -jar eventqueue.jar 100 100 test6/file1.in test6/file2.in test6/file3.in test6/file4.in
testoutput 6
echo "Test 6 completed successfully"

echo "Running test 7"
java -jar eventqueue.jar 300 200 test7/file1.in test7/file2.in test7/file3.in test7/file4.in
testoutput 7
echo "Test 7 completed successfully"
