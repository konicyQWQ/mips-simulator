# Mips模拟器

## 1. 设计思路

1. 利用C++模拟计算机CPU和内存，实现**产品主要功能**
2. 集成`汇编器`进入模拟器，实现指令的汇编和反汇编
3. 使用`QT`开发图形界面，方便用户操作

## 2. 原理证明

- 汇编器的证明已经在汇编器产品说明中完成
- CPU的模拟完全忠于寄存器和内存，其正确性在理论上是显然的，我们会在后面的部分通过测试样例来证明其实践上的正确性。

## 3. 程序架构

### 3.1 程序流程图

![流程图](D:\Programming\Computer System\mips-simulator\总体报告\img\流程图.png)

### 3.2 图形界面

模拟器界面利用`Qt`搭建，包括按钮、代码编辑框、控制台，以及内存、显存、寄存器的显示区域。

基础界面如下：

![image-20200630132558722](D:\Programming\Computer System\mips-simulator\石蒙.assets\image-20200630132558722.png)

在使用时，通过上方的按钮打开文件、编译运行，用户可以实时查看程序运行过程、内存变化、寄存器变化，并且可以在任何时候修改内存和寄存器。

## 4. 使用手册

### 4.1 编写与编译

#### 新建文件

首先单击`新建文件`，提示新建成功

![image-20200630132637079](D:\Programming\Computer System\mips-simulator\石蒙.assets\image-20200630132637079.png)

随后直接在代码编辑区域里输入代码即可

![image-20200630132834071](D:\Programming\Computer System\mips-simulator\石蒙.assets\image-20200630132834071.png)

#### 打开已有文件

![image-20200630133000954](D:\Programming\Computer System\mips-simulator\石蒙.assets\image-20200630133000954.png)

#### 编译

直接点编译`按钮`即可，提示编译成功，同时程序将会在运行程序目录下新建一个`binary_code.txt`文件，存放汇编代码生成的二进制码，作为以后的运行使用，请勿删除。

### 4.2 调试与运行

如果直接点击`运行`，程序会直接运行结束，并切换至显存界面供用户查看结果；如果点击`单步运行`，程序会一步一步运行，用户可以在运行过程中查看寄存器和内存。

![image-20200630134032360](D:\Programming\Computer System\mips-simulator\石蒙.assets\image-20200630134032360.png)

#### 跳转

内存无法用一页显示完全，因此我们提供了跳转显示功能，输入 10，单击跳转，可以看到此时内存显示区域已经从 10 开始了。

![image-20200630134130414](D:\Programming\Computer System\mips-simulator\石蒙.assets\image-20200630134130414.png)

#### 直接修改内存和寄存器

在调试时，为了方便用户，避免重复编写和编译，我们提供了直接修改内存和寄存器的功能。

输入 10 地址，输入修改的内容是 48，点击修改，可以发现结果已经变为 48。**注意这里的数字是 16 进制！**

![image-20200630134333231](D:\Programming\Computer System\mips-simulator\石蒙.assets\image-20200630134333231.png)

输入 2 寄存器，设置为 10，点击`设置`按键，可以看到寄存器已经发生了变化。

![image-20200630134458677](D:\Programming\Computer System\mips-simulator\石蒙.assets\image-20200630134458677.png)

尽管这项功能非常好用，但也请用户时刻注意，不要修改乱了内存。

## 5. 结果分析

经过测试，本产品功能完善且无bug。测试人员使用一段完整的汇编程序进行编译、运行、调试，均无任何错误。

测试样例如下：

```assembly
	   addi $t0,$zero,0
loop:  
       li $v0,1+4 #read_int
       syscall
       add $t0,$v0,$t0 #sum = sum + a
       bne $v0,$zero,loop
       li $v0,1
       move $a0,$t0 #print(sum)
       syscall
```

## 6. 技术指标

### 6.1 功能支持

本产品支持所有Mips标准指令、大量的Mips伪指令、格式指令、标签、表达式和变量。有关Mips的具体内容见附录。

本产品支持**汇编**语言的输入输出，类型包括整数、单/双精度浮点数、字符、字符串。

本产品具备完善且友好的用户界面，使用体验佳。

### 6.2 程序亮点：语法高亮

本产品的代码编辑部分实现了汇编语言的语法高亮功能，对用户编辑的代码进行分析，对各种关键字进行不同颜色的高亮提示，方便用户检查错误、分析代码。

我们采用了正则表达式分析代码，表达式的内容如下：

![image-20200630135235442](D:\Programming\Computer System\mips-simulator\石蒙.assets\image-20200630135235442.png)

经过高亮处理后的代码如下：

![image-20200630135808701](D:\Programming\Computer System\mips-simulator\石蒙.assets\image-20200630135808701.png)

可以看到，代码十分美观。

## 附录

> 人员分工

PM：孙恺元

- 孙恺元：CPU和内存的模拟、指令运行。
- 石蒙：用户界面、显示、代码高亮
- 杨琛：格式指令与变量处理

> 文件结构

```

```

> 参考资料

- [ZPC之MIPS指令集2019.pdf](./reference/ZPC之MIPS指令集2019.pdf)

