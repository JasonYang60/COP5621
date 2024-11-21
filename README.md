# Step 1

``` shell
make
```
# Step 2 
Incorrect Programs begin with number 0
```shell
./comp < 'incorrect program'/test00.txt  show semantic error: undeclared function name.
./comp < 'incorrect program'/test01.txt  show semantic error: numbers of arguments doesn't match with the number of parameters in function
./comp < 'incorrect program'/test02.txt  show semantic error: undefined variable name.
./comp < 'incorrect program'/test03.txt  show semantic error: conflicted function name.
./comp < 'incorrect program'/test04.txt  show semantic error: argument has the name of a defined function.
./comp < 'incorrect program'/test05.txt  show type error:  arguments of + do not type check.
./comp < 'incorrect program'/test06.txt  show type error:  arguments of % do not type check.
./comp < 'incorrect program'/test07.txt  show type error:  arguments of if do not type check.
./comp < 'incorrect program'/test08.txt  show type error:  argument type doesn't match with the parameter type.
```
Correct Programs begin with number 1
```shell
./comp < 'correct program'/test10.txt  The right version of test00.txt
./comp < 'correct program'/test11.txt  The right version of test01.txt
./comp < 'correct program'/test12.txt  The right version of test02.txt
./comp < 'correct program'/test13.txt  The right version of test03.txt
./comp < 'correct program'/test14.txt  The right version of test04.txt
./comp < 'correct program'/test15.txt  The right version of test05.txt
./comp < 'correct program'/test16.txt  The right version of test06.txt
./comp < 'correct program'/test17.txt  The right version of test07.txt
./comp < 'correct program'/test18.txt  The right version of test08.txt
./comp < 'correct program'/test19.txt  Show the logic of our program in handling variable name conflicts.
```
**Important**：When handling variable name conflicts, In our program we follow the principles of C-like compilers, and will ensure that inner-scope variables with the same name take precedence over outer-scope variables within their scope.


