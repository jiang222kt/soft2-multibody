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
  TStudent1 s4;
  
  /* 以下に各構造体の先頭アドレスを表示するプログラムを書く */
  /* Hint: printf で %p フォーマットを指定する*/
  /* 逆順に表示（s_end, s3, s2, s1, s_begin の順）*/
  printf("s_end : %p\n", &s_end);
  //s3について
  printf("s3 : %p\n", &s3);
  printf("TStudent2.tag : %p\n", &s3.tag);
  printf("TStudent2.id : %p\n", &s3.id);
  printf("TStudent2.name : %p\n", s3.name);
  printf("TStudent2.age : %p\n", &s3.age);
  printf("TStudent2.height : %p\n", &s3.height);
  printf("TStudent2.weight : %p\n", &s3.weight);
  //s2について
  printf("s2 : %p\n", &s2);
  printf("TStudent1.id : %p\n", &s2.id);
  printf("TStudent1.name : %p\n", s2.name);
  printf("TStudent1.age : %p\n", &s2.age);
  printf("TStudent1.height : %p\n", &s2.height);
  printf("TStudent1.weight : %p\n", &s2.weight);
  printf("TStudent1.tag : %p\n", &s2.tag);
  //s1について
  printf("s1 : %p\n", &s1);
  printf("Student.id : %p\n", &s1.id);
  printf("Student.name : %p\n", s1.name);
  printf("Student.age : %p\n", &s1.age);
  printf("Student.height : %p\n", &s1.height);
  printf("Student.weight : %p\n", &s1.weight);

  printf("s_begin : %p\n", &s_begin);
  printf("s4 : %p\n", &s4);
  // do_something!!

  /* 以下には各構造体のサイズをsizeof演算子で計算し、表示する */
  /* printf 表示には%zu を用いる*/
  printf("s_end : %zu\n", sizeof(s_end));
  printf("s3 : %zu\n", sizeof(s3));
  printf("s2 : %zu\n", sizeof(s2));
  printf("s1 : %zu\n", sizeof(s1));
  printf("s_begin : %zu\n", sizeof(s_begin));
  printf("char : %zu\n", sizeof(char));
  printf("int : %zu\n", sizeof(int));
  printf("double : %zu\n", sizeof(double));
  // do_something!!
  
  return 0;
}