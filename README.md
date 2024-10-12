# Step 1
Compile parser.y and scanner.l.
``` shell
yacc -d parser.y && flex scanner.l && gcc lex.yy.c y.tab.c -ll    
```
# Step 2 
Run test cases.
```shell
./a.out < test0.txt
./a.out < test1.txt
```
# workload description
Jason finished the parser.y
Lin finished the scanner.y and two test examples