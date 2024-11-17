# Step 1

``` shell
make
```
The warning "3 reduce/reduce conflicts" comes from reduction of empty token.
# Step 2 
Correct Samples.
```shell
./comp < test0.txt
./comp < test1.txt
./comp < test2.txt
```
Incorrect Sample.
```shell
./comp < test3.txt
```
Our code will report "syntax error" and skip the AST generation process.



