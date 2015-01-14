# 关于ShellCode的若干前置知识


## 介绍



## 一个简单的ShellCode
```
#include <stdio.h>
#include <stdlib.h>
#include "windows.h"
void test()
{
	MessageBox(NULL,"test","ShellCode",MB_OK);
}
int main(void)
{
	typedef void (* PF)();	
	unsigned char szBuf[256];
    unsigned char szBuf1[256]="\x6a\x00\x68\x38\x60\x40\x00\x68\x30\x60\x40\x00
    \x6a\x00\xff\x15\xa4\x50\x40\x00\xc3";
	//这就是shellcode,其实就是test的机器码，要制作shellcode其实不难，下面的while循环就可以把一段代码转换成，
	//难的是如何去执行我们的shellcode，在哪执行，如何把他变成漏洞利用，
	//当然这个地方要注意的是不是简单的把函数的汇编代码转换一下就行了，因为涉及程序访问数据的问题
	unsigned char *ps=(unsigned char *)&test; 
	unsigned char *pd=szBuf;
	PF pf=(PF)&szBuf[0];
	while(1)
	{
		*pd=*ps;
		if(*ps==0xc3)  //0xc3就是ret指令的机器码
		{
			break;
		}
		pd++,ps++;
	}
	pf();
	PF pf1=(PF)&szBuf1[0];
	pf1();
	return 0;
}

```

## 工具



## 编写



## 测试


## 利用Hash取值获取函数地址的ShellCode


## 参考

* Basic ShellCode Example - http://badishi.com/basic-shellcode-example/
* ShellCode教程从新手到高手 - http://drops.wooyun.org/tips/4225
* ShellCode编写之hash式函数调用及相关 - http://www.pediy.com/kssd/pediy10/58393.html
* http://www.vividmachines.com/shellcode/shellcode.html