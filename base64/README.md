# base64

C++ 写的一个关于base64编码，解码二进制流的例子，编译后可以使用命令行运行example，手工编写的命令行参数识别，所以写的很垃圾，有时间再填坑。

## Run
编译:：
```bash
g++ base64.cpp -o base64 -O2
```
运行：
```bash
base64 -e 中文 #编码字符串
base64 -d YQ== #解码字符串
base64 -ef h -i base64.cpp -o temp #将base64.cpp文件使用base64编码，并写入到临时文件temp
base64 -df h -i temp -o test.cpp #将临时文件temp(base64编码)解码，并写入到test.cpp
```