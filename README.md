# Step 1

``` shell
make
```
# Step 2 
Tests begin with number 1 will make the semantic checking, such as test10.txt.


Tests from test10 to test14 are the incorrect programs.

Tests from test15 to test19 are the corresponding correct program versions.

```shell
./comp < test10.txt  **Function name undeclared. **
./comp < test11.txt  **Number of arguement doesn't match with function.**
./comp < test12.txt  **Arguement type conflicts.**
./comp < test13.txt  **Variable name undefined.**
```
Incorrect Sample.
```shell
./comp < test3.txt
```
Our code will report "syntax error" and skip the AST generation process.



