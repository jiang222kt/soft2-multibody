# 考察
## コード#1
```
#include <stdio.h>

typedef struct student
{
  int id;
  char name[100];
  int age;
  double height;
  double weight;
} Student;

typedef struct tagged_student1
{
  int id;
  char name[100];
  int age;
  double height;
  double weight;
  char tag;
} TStudent1;

typedef struct tagged_student2
{
  char tag;
  int id;
  char name[100];
  int age;
  double height;
  double weight;
} TStudent2;

int main (int argc, char**argv)
{
  Student s_begin;
  Student s1;
  TStudent1 s2;
  TStudent2 s3;
  TStudent2 s_end;

  //アドレスを表示
  printf("s_end : %p\n", &s_end);
  printf("s3 : %p\n", &s3);
  printf("s2 : %p\n", &s2);
  printf("s1 : %p\n", &s1);
  printf("s_begin : %p\n", &s_begin);

  //サイズを表示
  printf("s_end : %zu\n", sizeof(s_end));
  printf("s3 : %zu\n", sizeof(s3));
  printf("s2 : %zu\n", sizeof(s2));
  printf("s1 : %zu\n", sizeof(s1));
  printf("s_begin : %zu\n", sizeof(s_begin));
  printf("char : %zu\n", sizeof(char));
  printf("int : %zu\n", sizeof(int));
  printf("double %zu\n", sizeof(double));
}
```

## 実行結果
```
s_end : 0x7ffc7cb8a520
s3 : 0x7ffc7cb8a4a0
s2 : 0x7ffc7cb8a5a0
s1 : 0x7ffc7cb8a420
s_begin : 0x7ffc7cb8a3a0
  
s_end : 128
s3 : 128
s2 : 136
s1 : 128
s_begin : 128
  
char : 1
int : 4
double : 8
```

## ここからわかること
まず、charは1バイト、intは4バイト、doubleは8バイトなので、Student, TStudent1, TStudent2の必要となるメモリはそれぞれ124バイト、125バイト、125バイトとなるはずである。しかし、実際に確保されているメモリを表示してみると、128バイト、136バイト、128バイトである。これは、チャンクによるものである。実際に確保されているメモリを見てみると、全て8バイトの倍数となっている。CPUがメモリからデータを読み出す際に、まとまった単位(チャンク:ここでは8バイト)でやりとりするからである。つまり、空のデータ領域を途中で挿入する**パディング**を行い、特定のメモリアドレスの倍数にデータが配置されるように調整(**アライメント**)されている。
  
ここで、表示された各構造体のアドレスの下3桁を16進数表示から10進数表示に直してみる。
```
s2 : 0x7ffc7cb8a5a0      //5a0 → 1440 = 1312 + 128
s_end : 0x7ffc7cb8a520   //520 → 1312 = 1184 + 128
s3 : 0x7ffc7cb8a4a0      //4a0 → 1184 = 1056 + 128
s1 : 0x7ffc7cb8a420      //420 → 1056 = 928 + 128
s_begin : 0x7ffc7cb8a3a0 //3a0 → 928
```
これだけだとs2がどれだけメモリを確保しているのかが見えないため、先ほどのコードにおいて新たにs4を作りそれを表示した。冗長になってしまうため、main関数の一部のみを記す。

```
int main (int argc, char**argv)
{
  Student s_begin;
  Student s1;
  TStudent1 s2;
  TStudent2 s3;
  TStudent2 s_end;
  TStudent1 s4;

  //アドレスを表示
  printf("s_end : %p\n", &s_end);
  printf("s3 : %p\n", &s3);
  printf("s2 : %p\n", &s2);
  printf("s1 : %p\n", &s1);
  printf("s_begin : %p\n", &s_begin);
  printf("s4 : %p\n", &s4);
}
```
これを実行し、アドレスの下3桁を10進数に変換して大きさ順に並び替えると以下のようになった。
```
s4 : 0x7ffcf871d980      //980 → 2432 = 2288 + 144
s2 : 0x7ffcf871d8f0      //8f0 → 2288 = 2160 + 128
s_end : 0x7ffcf871d870   //870 → 2160 = 2032 + 128
s3 : 0x7ffcf871d7f0      //7f0 → 2032 = 1904 + 128
s1 : 0x7ffcf871d770      //770 → 1904 = 1776 + 128
s_begin : 0x7ffcf871d6f0 //6f0 → 1776
```
これを見ると、他の構造体は確かに128バイト分のメモリを確保しているのがわかるが、s2は136バイト確保しているにもかかわらず、s2とその次の構造体の間には144バイトの領域がある。つまり8バイト分パディングされていることがわかる。

では、なぜs2だけ他の構造体と違って136バイト確保されているのだろうか。次に、各構造体中のメンバ変数のアドレスを表示して、そのうちのs2とs3に注目するため該当部分のコードのみを記す。

## コード#2
```
#include <stdio.h>

typedef struct student
{
  int id;
  char name[100];
  int age;
  double height;
  double weight;
} Student;

typedef struct tagged_student1
{
  int id;
  char name[100];
  int age;
  double height;
  double weight;
  char tag;
} TStudent1;

typedef struct tagged_student2
{
  char tag;
  int id;
  char name[100];
  int age;
  double height;
  double weight;
} TStudent2;

int main (int argc, char**argv)
{
  Student s_begin;
  Student s1;
  TStudent1 s2;
  TStudent2 s3;
  TStudent2 s_end;

  //s2について
  printf("s2 : %p\n", &s2);
  printf("TStudent1.id : %p\n", &s2.id);
  printf("TStudent1.name : %p\n", s2.name);
  printf("TStudent1.age : %p\n", &s2.age);
  printf("TStudent1.height : %p\n", &s2.height);
  printf("TStudent1.weight : %p\n", &s2.weight);
  printf("TStudent1.tag : %p\n", &s2.tag);

  //s3について  
  printf("s3 : %p\n", &s3);
  printf("TStudent2.tag : %p\n", &s3.tag);
  printf("TStudent2.id : %p\n", &s3.id);
  printf("TStudent2.name : %p\n", s3.name);
  printf("TStudent2.age : %p\n", &s3.age);
  printf("TStudent2.height : %p\n", &s3.height);
  printf("TStudent2.weight : %p\n", &s3.weight);
}
```

## 実行結果(右に、下3桁を10進数表示に変換した値も記す)
```
s2 : 0x7ffcf7735da0               //3488
TStudent1.id : 0x7ffcf7735da0     //3488
TStudent1.name : 0x7ffcf7735da4   //3492
TStudent1.age : 0x7ffcf7735e08    //3592
TStudent1.height : 0x7ffcf7735e10 //3600
TStudent1.weight : 0x7ffcf7735e18 //3608
TStudent1.tag : 0x7ffcf7735e20    //3616
s3 : 0x7ffcf7735d20               //3360
TStudent2.tag : 0x7ffcf7735d20    //3360
TStudent2.id : 0x7ffcf7735d24     //3364
TStudent2.name : 0x7ffcf7735d28   //3368
TStudent2.age : 0x7ffcf7735d8c    //3468
TStudent2.height : 0x7ffcf7735d90 //3472
TStudent2.weight : 0x7ffcf7735d98 //3480
```

## ここからわかること
