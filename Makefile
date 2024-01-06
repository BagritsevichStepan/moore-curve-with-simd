#CC defines compiler
CC = gcc
#Flags of compiler
CFLAGS = -Wall -O3
#Files to be compiled into the one executable file
SOURCES = main_program.c moore_curve_fast.c moore_curve_gray_code.c moore_curve_recursive.c
#Executable file that can be run
EXECUTABLE = moore_curve

all:
	$(CC) $(CFLAGS) $(SOURCES) -o $(EXECUTABLE)

#Run to get help info
help: all
	./$(EXECUTABLE) --help


#Run test like in input1.txt
test1: all
	./$(EXECUTABLE) -n 1 -o output1.txt

#Run test like in input2.txt
test2: all
	./$(EXECUTABLE) -n 2 -o output2.txt

#Run test like in input3.txt
test3: all
	./$(EXECUTABLE) -n 3 -o output3.txt

#Run test like in input4.txt
test4: all
	./$(EXECUTABLE) -n 4 -o output4.txt


#Possible executable degrees of moore curve
DEGREES = 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16

#Runs iterative solution for all DEGREES
run_all_iterative: all
	$(foreach var,$(DEGREES),./$(EXECUTABLE) -n $(var) -B 1 -o output.txt;)
#Runs gray code solution for all DEGREES
run_all_gray_code: all
	$(foreach var,$(DEGREES),./$(EXECUTABLE) -V 1 -n $(var) -B 1 -o output.txt;)
#Runs recursive solution for all DEGREES
run_all_recursive: all
	$(foreach var,$(DEGREES),./$(EXECUTABLE) -V 2 -n $(var) -B 1 -o output.txt;)


#Use to clean folder from binary files
clean:
	rm -rf *.o $(EXECUTABLE)

