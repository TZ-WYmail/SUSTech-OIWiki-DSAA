## 函数与指针
c++ 的函数只能返回一个值，不能返回多个值。
但是可以通过指针来实现多个值的返回。
形参为指针的时候一般有两个目的：
1. 实参的类型较大，例如数组、结构体等。
2. 实参需要在函数中被修改。

例：给定两个正整数，要求同时返回商和余数。
```cpp:
bool divmod(int a,int b,int* x,int* y){
    if(b==0) return false;
    *x=a/b;
    *y=a%b;
    return true;
}

int main(){
    int a,b;
    cin>>a>>b;
    int x,y;
    bool t=divmod(a,b,&x,&y);
}
```
char* p指向了一个c串，要得到把他复制3倍的串，要多大的空间：
```cpp:
int len=strlen(p);
char* q=new char[len*3+1];
```

## 函数的递归：略
## 宏与内联函数：
c的遗产，尽量的不使用宏。
程序中的常量，尽量使用const

内联函数：
```cpp:
inline int max(int a,int b){
    return a>b?a:b;
}
```
缺点：
1. 内联函数的代码会被复制到调用的地方，增加了代码的大小。
2. 内联函数不能递归调用。


## 结构体：
结构体是一种用户自定义的数据类型，它可以包含多个不同类型的变量。
结构体的每个变量称为成员变量。
结构体的每个成员变量都有一个名称，称为成员名。
结构体的每个成员变量都有一个类型，称为成员类型。

```cpp:
struct student{
    int id;
    char name[20];
    int age;
    char sex[10];
};
int main(){
    student s;
    cin>>s.id>>s.name>>s.age>>s.sex;
    cout<<s.id<<" "<<s.name<<" "<<s.age<<" "<<s.sex;
}
```
“class 默认私有，struct 默认公有”——这是 C++ 里唯一语言层面的差异，其余特性（继承、多态、模板、友元、构造函数、虚函数……）二者完全等价。


## 文件：
```cpp:
# include <iostream>
# include <fstream>
using namespace std;
int main(){
    ifstream in("input.txt");
    if(!in){
        cout<<"error";
        return 0;
    }
    ofstream out("output.txt");
    int a,b;
    in>>a>>b;
    out<<a+b;
    in.close();
    out.close();
    return 0;
}

int x;
while (in >> x) {          // 成功读到继续，失败就停
    // 处理 x
}

只要到达文件尾 或 遇到非法格式，>> 返回的流对象就会隐式转 false，循环立刻结束。
不需要 eof()、good() 之类函数。

std::string line;
while (std::getline(in, line)) {   // 成功读到一行继续
    // 处理 line
}

读到文件尾时 getline 返回 false，循环结束。
如果最后一行没有换行符，getline 仍会把这一行读出来并返回 true，下一次再读才返回 false，所以不会漏数据。

```
注意文件读写的循环写法：
while (!in.eof()) { ... }   // 错！会多读一次

eof() 只在真正尝试读并失败后才变为 true，提前判断会造成最后一次循环处理空数据。

## 串的读入：
```cpp:
int main(){
    ifstream("3.txt");
    char s[100];
    while(in.getline(s,sizeof(s))){
        cout<<s<<endl;
    }
}
```
## 重定向：
```cpp:
int main(){
    freopen("3.txt","r",stdin);
    freopen("4.txt","w",stdout);
    int a,b;
    cin>>a>>b;
    cout<<a+b;
    // 关闭文件
    fclose(stdin);
    fclose(stdout);
}
```

## 输入输出格式控制：
下面给出一份“竞赛/笔试 100% 够用”的 `printf`/`scanf` 超全速查表。  
每个知识点都给出**可复制运行**的示例，记住这些，你就能在 5 分钟内格式输出/读入任何常见数据。

---

## 一、printf 家族（格式化输出）

### 1. 原型与头文件
```cpp
#include <cstdio>          // 或 <stdio.h>
int printf(const char *fmt, ...);
```

### 2. 最常用 6 个转换说明
| 说明符 | 含义 | 示例 |
|---|---|---|
| `%d` | 十进制有符号 int | `printf("%d\n", 123);` |
| `%u` | 十进制无符号 unsigned | `printf("%u\n", 4294967295U);` |
| `%lld` | long long | `printf("%lld\n", 1LL<<40);` |
| `%llu` | unsigned long long | `printf("%llu\n", 1ULL<<63);` |
| `%f` | double（默认 6 位小数） | `printf("%f\n", 3.1415926535);` |
| `%lf` | 与 `%f` 完全等价（C99 起） | `printf("%lf\n", 3.14);` |

### 3. 宽度、精度、对齐、补零
```cpp
int a = 5;
printf("[%5d]\n", a);       // 右对齐，宽度 5 → [    5]
printf("[%-5d]\n", a);      // 左对齐 → [5    ]
printf("[%05d]\n", a);      // 补零 → [00005]

double pi = 3.1415926;
printf("%.3f\n", pi);       // 保留 3 位小数 → 3.142
printf("%10.2f\n", pi);     // 总宽 10，2 位小数 → [     3.14]
```

### 4. 进制与指针
```cpp
unsigned x = 255;
printf("%#x %#X\n", x, x);  // 0xff 0XFF   (# 带前缀)
printf("%o\n", x);          // 八进制 → 377
int *p = &a;
printf("%p\n", (void*)p);   // 指针地址
```

### 5. 字符与字符串
```cpp
char c = 'A';
char s[] = "hello";
printf("%c %s\n", c, s);    // A hello
printf("%.3s\n", s);        // 截取前 3 字符 → hel
```

### 6. 转义序列
```cpp
printf("%%\n");             // 输出一个 %
printf("\\\n");             // 输出一个 \
printf("abc\ndef\n");       // 换行
```

### 7. 返回值
成功输出的**字符数**；负数表示出错。
```cpp
int len = printf("%d\n", 42);   // len == 3（含 \n）
```

---

## 二、scanf 家族（格式化输入）

### 1. 原型
```cpp
int scanf(const char *fmt, ...);
```
返回**成功读入的字段个数**；读到文件尾返回 `EOF`（通常是 -1）。

### 2. 最常用 6 个转换说明
| 说明符 | 含义 | 示例 |
|---|---|---|
| `%d` | int | `scanf("%d", &x);` |
| `%u` | unsigned | `scanf("%u", &ux);` |
| `%lld` | long long | `scanf("%lld", &ll);` |
| `%llu` | unsigned long long | `scanf("%llu", &ull);` |
| `%f` | float | `scanf("%f", &f);` |
| `%lf` | double | `scanf("%lf", &d);` |

### 3. 字符与字符串
```cpp
char c;
scanf(" %c", &c);      // 注意前面空格，跳过空白

char s[100];
scanf("%s", s);        // 读到空格/换行结束，自动补 '\0'
printf("%s\n", s);
```

### 4. 带宽度限制——防止溢出
```cpp
char s[16];
scanf("%15s", s);      // 最多读 15 字符 + '\0'，安全
```

### 5. 读一整行（含空格）
```cpp
char line[256];
scanf("%[^\n]%*c", line);   // 读到 '\n' 为止，%*c 把 '\n' 吃掉
```
> 更推荐 C++ 的 `std::getline`，但纯 C 环境就用上面写法。

### 6. 跳过固定字符
```cpp
int h, m, s;
scanf("%d:%d:%d", &h, &m, &s);   // 输入 23:59:59 也能正确解析
```

### 7. 连续读直到文件结束
```cpp
int x;
while (scanf("%d", &x) == 1) {   // 返回 1 表示成功读到 1 个字段
    // 处理 x
}
```

---

## 三、易错点黑名单（必须记住）

| 错误写法 | 后果 |
|---|---|
| `printf("%d", 3.14);` | 未定义行为，可能崩溃 |
| `scanf("%d", x);` | 传值而非地址，段错误 |
| `scanf("%s", &s);` | s 本身就是数组地址，再取地址错 |
| `scanf("%d\n", &x);` | 结尾 `\n` 会阻塞等待非空白字符，卡输入 |
| `while (!scanf(...)) ;` | 返回 0 表示匹配失败，不是 EOF |

---