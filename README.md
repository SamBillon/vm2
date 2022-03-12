# vm2
vm2 C++实现的一个简单的虚拟机和它配套的汇编器

## 使用方法
### vm.cpp
克隆本仓库到本地，之后执行
```
g++ vm.cpp -o vm.exe
./vm.exe --help
```
查看使用教程。
或：
```
g++ vm.cpp -o vm.exe
./vm.exe
```
程序默认将尝试从同目录下的`bin_code.o`读取二进制代码，并将运行结果输出到标准输出流（如有输出）。
### assembler.cpp
克隆本仓库到本地，之后执行
```
g++ assembler.cpp -o assembler.exe
./assembler.exe --help
```
查看使用教程。
或：
```
g++ assembler.cpp -o assembler.exe
./assembler.exe
```
程序默认将尝试从同目录下的`code.txt`读取汇编，并将二进制代码输出到同目录下`bin_code.o`。

## 配套汇编指令集与对应二进制指令
通常，用形如`rx`的记号来标记寄存器`R[x]`，用形如'x#'的记号来标记存储器'M[x]'。
### 标记
标记是以`:`结尾的任意字符串（例如`L0:`）。标记不会被汇编器解释，但之后可作为`jmp`指令跳转的目标。
### R型指令
R型指令是形如`00000010 00010010`的指令。其中前8位为操作码`op`，之后4位为目标寄存器与第一操作寄存器，最后4位为第二操作寄存器。
R型指令的作用是寄存器之间的操作。
```
操作码: 对应指令与参数: 作用
00000000: mov rx, ry: R[x] <- R[y]
00000001: add rx, ry: R[x] <- R[x] + R[y]
00000010: sub rx, ry: R[x] <- R[x] - R[y]
00000011: mul rx, ry: R[x] <- R[x] * R[y]
00000100: ifsm rx, ry: 执行这条指令的下条指令当且仅当 R[x] < R[y]
00000101: set rx, y: R[x] <- y
```
### M型指令
M型指令是形如`11111110 00001011`的指令。其中前8位为操作码`op`，之后8位为目标存储器。
M型指令的作用是寄存器与存储器之间的操作。
注意：所有M型指令都需要寄存器`r0`参与，所以所有M型指令都必须有`r0`作为参数。
```
操作码: 对应指令与参数: 作用
11111111: store x#, r0: M[x] <- R[0]
11111110: load r0, x#: R[0] <- M[x]
```
### I型指令
I型指令是和M型指令结构相似的指令，不同的是I型指令后8位不一定代表寄存器或存储器。
```
操作码: 对应指令与参数: 作用
11001111: output rx: 输出R[x]的值
11001110: input rx: 从标准输入流获取输入并保存到R[x]
11001101: jmp L0: 将程序计数器pc调整到标记L0处
```
### 示例汇编程序
这是一个基于本指令集的汇编程序，它从标准输入流读取一个整数`t`，并将重复`t`次如下过程：
从标准输入流读取两个整数`x, y`，计算它们的和并输出到标准输出流。
```
    set r2, 1
    input r3
L0: input r0
    input r1
    add r0, r1
    output r0
    add r4, r2
    ifsm r4, r3
    jmp L0
 ```
 用汇编器得到如下二进制代码：
 ```
00000101 00100001
11001110 00000011
11001110 00000000
11001110 00000001
00000001 00000001
11001111 00000000
00000001 01000010
00000100 01000011
11001101 00000010
```
