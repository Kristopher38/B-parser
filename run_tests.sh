#!/bin/bash

DIFF_OPTIONS="--context=10 --ignore-trailing-space --ignore-blank-lines"
NPP_PATH="/c/Program Files/Notepad++/notepad++.exe"
BUILD_NAME="Debug"

if [ -z $1 ]
then
	echo "Missing build name argument, assuming $BUILD_NAME"
else
	BUILD_NAME="$1"
fi

EXEC_PATH="../build-compiler-Desktop-$BUILD_NAME"

cd "test_cases"

for filename in *.txt; do
	cd "/c/Users/Krzychu/Desktop/compiler/test_cases"
	TOPLINE="$(head -n 1 $filename)"
	TITLE="$(echo -e "${TOPLINE///}" | sed -e 's/^[[:space:]]*//')"
	BASENAME="$(basename $filename .txt)"
	DIFF_FILE="$PWD/$BASENAME.diff"
	SOURCE_FILE="$PWD/$BASENAME.txt"
	FAIL_FILE="$PWD/$BASENAME.fail"

	cd "$EXEC_PATH"
	echo "Running test $filename"
	echo "$TITLE"

	RUN_TEST_CMD="./compiler.exe --testcase $SOURCE_FILE"
	TEST_OUTPUT="$(diff $DIFF_OPTIONS $DIFF_FILE <($RUN_TEST_CMD))"

	RETVAL="$?"

	if [ $RETVAL -eq 0 ]
	then
		echo "TEST PASSED"
		if [ -e $FAIL_FILE ]
		then
			rm "$FAIL_FILE"
		fi
	elif [ $RETVAL -eq 1 ]
	then
		echo "TEST FAILED, diff available in $FAIL_FILE"
		echo "$TEST_OUTPUT" > "$FAIL_FILE"
		cd "../test_cases"
		nohup "$NPP_PATH" "$BASENAME.fail" &>/dev/null &
	else
		echo "TEST FAILED, error diffing files"
	fi
done
