使用正则匹配形如: `void return_void() {` 的子串
之后将最后一个花括号之前的内容作为log，插入到文件中

例如，源文件:
```
void return_void(const int &x, const char *argv[]){ return; }

int main(){return 0;}
```
使用`logger()`操作后变为:
```
void return_void(const int &x, const char *argv[]){ logger(void return_void(const int &x, const
char *argv[])); return; }

int main(){logger("int main()"); return 0;}
```

