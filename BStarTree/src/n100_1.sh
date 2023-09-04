# !/bin/bash
TESTCASE_NAME="n100"
echo "0.1" >> ${TESTCASE_NAME}.csv
for i in {1..10000};do
    ./../bin/hw3 ../testcases/${TESTCASE_NAME}.hardblocks ../testcases/${TESTCASE_NAME}.nets ../testcases/${TESTCASE_NAME}.pl ../output/${TESTCASE_NAME}.floorplan 0.1 $i >> ${TESTCASE_NAME}_99_1.csv
done