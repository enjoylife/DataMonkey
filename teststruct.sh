echo "Running unit tests:"

for i in $1
do
    if test -f $i
    then
        ./$i 2 # > tests/tests.log
        #if ./$i 2 # > tests/tests.log
        #then
            #echo Passed
            # cat tests/tests.log
        #else
        #    echo "ERROR in test $i: here's tests/tests.log"
        #    echo "------"
        #    tail tests/tests.log
        #    exit 1
        #fi
    fi
done

echo ""