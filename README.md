# Step 1

``` shell
make
```
# Step 2 
Tests begin with number 1 will make the semantic checking, such as test10.txt.


Tests from test10 to test13 are the incorrect programs.

Tests from test14 to test15 are the correct programs.

```shell
./comp < test10.txt  Function name undeclared. 
./comp < test11.txt  Number of arguement doesn't match with function.
./comp < test12.txt  Variable name undefined.
./comp < test13.txt  Arguement has the name of a defined function.
./comp < test14.txt Correct
./comp < test15.txt Correct
```
Tests begin with number 2 will make the semantic checking, such as test20.txt.


Tests from test20 to test22 are the incorrect programs.

Test test23 is the correct programs.

```shell
./comp < test20.txt  arg types of + should be INT.
./comp < test21.txt  arg types of div should be INT.
./comp < test22.txt  types of the last two args of if should be the same.
./comp < test23.txt  Correct
```





