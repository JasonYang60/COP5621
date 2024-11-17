# Step 1

``` shell
make
```
# Step 2 
Incorrect Programs begin with number 0
```shell
./comp < test00.txt  show semantic error: undeclared function name.
./comp < test01.txt  show semantic error: numbers of arguments doesn't match with the number of parameters in function
./comp < test02.txt  show semantic error: undefined variable name.
./comp < test03.txt  show semantic error: conflicted function name.
./comp < test04.txt  show semantic error: argument has the name of a defined function.
./comp < test05.txt  show type error:  arguments of + do not type check.
./comp < test06.txt  show type error:  arguments of % do not type check.
./comp < test07.txt  show type error:  arguments of if do not type check.
./comp < test08.txt  show type error:  argument type doesn't match with the parameter type.
```
Correct Programs begin with number 1
```shell
./comp < test10.txt  The right version of test00.txt
./comp < test11.txt  The right version of test01.txt
./comp < test12.txt  The right version of test02.txt
./comp < test13.txt  The right version of test03.txt
./comp < test14.txt  The right version of test04.txt
./comp < test15.txt  The right version of test05.txt
./comp < test16.txt  The right version of test06.txt
./comp < test17.txt  The right version of test07.txt
./comp < test18.txt  The right version of test08.txt
./comp < test19.txt  Show the logic of our program in handling variable name conflicts.
```
**Important**：When handling parameter name conflicts, In our program we follow the principles of C-like compilers, and will ensure that inner-scope variables with the same name take precedence over outer-scope variables within their scope.


