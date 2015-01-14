# Poison Ivy ShellCode

## Pi的先进性
1. 多个DNS轮询解析目标IP，支持分组，及通过Socks4代理上线
2. 可以生成4种形式的ShellCode，体积小巧未压缩刚6k左右

## 分析方法
### 生成PE格式的bin
基本信息
```
区段: .text .data
导入表: Kernel32.dll|ExitProcess

```
OD进入之后只有一个call 400400
sub_401146(int a)
sub_400a70(int a1,int a2,int a3)

## 通用ShellCode模版

```
#include <windows.h>

unsigned char ShellCode[] =
{ 0x55,0x8B,0xEC,0x81,0xC4, //PI生成的二进制放在这里
......
0x00,0xC9,0xC2,0x04,0x00 //
};

int main()
{
((void(*)(void))&ShellCode)();
//也可以使用内嵌asm
_asm
{
lea eax,ShellCode
call eax
}
return 0;
}

```

## 参考
* scdbg - https://github.com/dzzie/SCDBG
* 非官方bin下载 - https://samsclass.info/123/proj10/p11-PI.htm