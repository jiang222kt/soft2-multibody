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
TStudent1.name : 0x7ffcf7735da4   //3492 = 3488 + 4
TStudent1.age : 0x7ffcf7735e08    //3592 = 3492 + 100
TStudent1.height : 0x7ffcf7735e10 //3600 = 3592 + 8
TStudent1.weight : 0x7ffcf7735e18 //3608 = 3600 + 8
TStudent1.tag : 0x7ffcf7735e20    //3616 = 3608 + 8
s3 : 0x7ffcf7735d20               //3360
TStudent2.tag : 0x7ffcf7735d20    //3360
TStudent2.id : 0x7ffcf7735d24     //3364 = 3360 + 4
TStudent2.name : 0x7ffcf7735d28   //3368 = 3364 + 4
TStudent2.age : 0x7ffcf7735d8c    //3468 = 3368 + 100
TStudent2.height : 0x7ffcf7735d90 //3472 = 3468 + 4
TStudent2.weight : 0x7ffcf7735d98 //3480 = 3472 + 8
```

## ここからわかること
s2では構造体全体の確保メモリが136バイトなので、s2のtagの確保メモリは8バイトであることがわかる。これを踏まえて、s2とs3のそれぞれのメンバ変数の確保メモリの違いを見てみると、ageとtagでの違いに気づける。s2のtagは8バイト確保しているのに対してs3のtagは4バイト確保しており、また、s2のageは8バイト確保しているのに対してs3のageは4バイト確保している。
  
では、それぞれの構造体について詳しく見ていこう。
  
まず、s2について、idとnameで104バイト確保していて、これはチャンク(8バイト)の倍数である。また、ageのあとのheightの型がdoubleで8バイト確保されるので、idとnameとint型のageの合計バイト数が8の倍数であることが好ましい。よって、ageについてはint型で4バイトしか必要ないにもかかわらず、4バイトパディングして8バイト確保されている。同様に、s2のtagまでに128バイトと8の倍数分のバイト数が確保されているので、tagでは7バイトパディングして8バイト確保されている。
  
次に、s3では最初にtagがあり、次にint型のidがある。この二つを合計して8バイトに収まるので、tagでは3バイトパディングして4バイト確保されている。そのあとnameで100バイト確保されて、ここまでで108バイトと8の倍数にならないが、その次のageで4バイト確保されて112バイトとなるので、この構造体ではageは4バイト確保される。

つまり、次に来るメンバ変数のバイト数を考慮して、8バイトの倍数になるようにアライメントされるのではないかと推察できる。次に、s3でのtagをもう一つ増やして先頭につけた以下のような構造体を考えてみる。仮説が正しければ、tag1とtag2とidが8バイトに収まるので、tag1は1バイト、tag2はバイトパディングして3バイト、idは4バイト確保されるはずである。
```
#include <stdio.h>

typedef struct tagged_student3
{
  char tag1;
  char tag2;
  int id;
  char name[100];
  int age;
  double height;
  double weight;
} TStudent3;

int main (int argc, char**argv){

  TStudent3 s4;

  printf("s4 : %p\n", &s4);
  printf("TStudent3.tag1 : %p\n", &s4.tag1);
  printf("TStudent3.tag2 : %p\n", &s4.tag2);
  printf("TStudent3.id : %p\n", &s4.id);
  printf("TStudent3.name : %p\n", s4.name);
  printf("TStudent3.age : %p\n", &s4.age);
  printf("TStudent3.height : %p\n", &s4.height);
  printf("TStudent3.weight : %p\n", &s4.weight);
}
```
下3桁を10進数表示に変換した値を含む実行結果は以下のようになった。
```
s4 : 0x7ffd7c997100               //100 → 256
TStudent3.tag1 : 0x7ffd7c997100   //100 → 256
TStudent3.tag2 : 0x7ffd7c997101   //101 → 257 = 256 + 1
TStudent3.id : 0x7ffd7c997104     //104 → 260 = 257 + 3
TStudent3.name : 0x7ffd7c997108   //108 → 264 = 260 + 4
TStudent3.age : 0x7ffd7c99716c    //16c → 364 = 264 + 100
TStudent3.height : 0x7ffd7c997170 //170 → 368 = 364 + 4
TStudent3.weight : 0x7ffd7c997178 //178 → 376 = 368 + 8
```
これを見ると、確かにtag1は1バイト、tag2は3バイト、idは4バイト確保されていて、仮説が正しいと考えられる。
## まとめ
アライメントのルールは、後ろのメンバ変数のバイト数を考慮し、前からメンバ変数のバイト数を足し合わせて8の倍数になるようにパディングされるということがわかった。