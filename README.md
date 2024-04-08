# myShell
we use echo to write the list of 


For wildcard, I try to find the list of files starting with m* 
with the following command 
ls -l m*
---
Redirection
try to write the list of items into the testfile by using
echo 
"Apple
Banh Mi
Beans
Candy
Mango
Mango
Pho" > test.txt


->  cat < test.txt
Finally, I for the task of Output the file out to the terminal, single command can redirect both standard input and standard output. 

grep Apple < test1.txt > test2.txt

---
Finally, for the Pipes to test the command that allows for a single pipe connecting two processes. To sort the items in the text file in reverse alphabetical order. 

cat test.txt | sort -r 