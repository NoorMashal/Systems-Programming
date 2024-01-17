NetID: nmm229

TESTS :-

Batch mode (in main terminal) :

Input: ./mysh myscript1.sh
Output: hello

Input: ./mysh myscript2.sh
Output: 2

Input: ./mysh myscript3.sh
Output: This is the input inside the text file

Input: ./mysh myscript4.sh
Output: A text file containing "Test message"

Input: ./mysh myscript5.sh
Output: foofuh.txt  input.txt  output.txt  README.txt

Input: ./mysh myscript6.sh
Output: myscript1.sh  myscript2.sh  myscript3.sh  myscript4.sh  myscript5.sh  myscript6.sh  mysh  mysh.c  mysh.o  output.txt

============================================================================================================================================================

Interactive mode (./mysh):

1. Test ls command 3 times, test cd command 3 times without an argument, test cd .. and try going into another file and check ls to see if you can access these files. 
   Get back to main file (where you started) to proceed the remaining tests. This was all to check for memory leaks.

2. Command: echo "Hello World" | wc -w
   Output: 2 

3. Command: cat < input.txt 
   Output: This is the input inside the text file (The text inside input.txt)

4. Command: echo "Test message" > output.txt
   Output: A text file containing "Test message"

5. TESTING FOR IF ELSE THEN

Test for then:
echo ls *.txt
then echo "Found text files" 
else echo "Did not find text files"
Final output: "Found text files" (assuming there are text files in the current working directory)

Test for else:
ls *.noor
then echo "Found .noor files"
else echo "Did not find .noor files"
Final output: "Did not find .noor files"

6. Repeat steps 1 2 3 4 and 5, 10 times to check for memory leaks again. Sometimes changing the text inside the text files to see if it updates correctly.

7. Finally, use the "exit" command to properly terminate mysh and it will raise any memory leaks (if there are any) on the way out. 