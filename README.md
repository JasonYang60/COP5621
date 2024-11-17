# Step 1

``` shell
make
```
# Step 2 
Incorrect Programs begin with number 0
```shell
./comp < test00.txt  show semantic error: undeclared function name.
./comp < test01.txt  show semantic error: numbers of arguments doesn't match with the number of parameters in function
./comp < test02.txt
```
Incorrect Sample.
```shell
./comp < test3.txt
```
Our code will report "syntax error" and skip the AST generation process.



