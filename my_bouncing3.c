#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "physics2.h"
#include <string.h>
#include <ctype.h>
#include <time.h>
#define FUSION_DISTANCE 3

int main(int argc, char **argv)
{
  const Condition cond =
    {
     .width  = 75,
     .height = 40,
     .G = 1.0,
     .dt = 1.0,
     .cor = 0.8
    };
  
  size_t objnum = 2;
  
  /* コマンドラインに引数があるか？のフラグ */
  int read_from_file = 0;

  /* ファイルを引数にとるか、ない場合はデフォルトの初期値を使う */
  if ( argc > 3 ) {
    fprintf(stderr, "usage: %s [物体の個数] [物体の質量と座標情報ファイル]\n", argv[0]);
    return EXIT_FAILURE;
  }
  else if (argc == 3) {
    /* 一番目の引数が数値か、チェックする　*/
    for(int i = 0; i < strlen(argv[1]); i++){
      if (!isdigit(argv[1][i])){
        fprintf(stderr, "%s is not a number\n", argv[1]);
        return EXIT_FAILURE;
      }
    }

    /* 文字列を数値に変換のうえ、objnumに設定する */
    objnum = atoi(argv[1]);

    read_from_file = 1;
  }
  else{
    objnum = 2;  // デフォルトの初期値を使う/
  }

  Object objects[objnum];

  if ( read_from_file ){

    FILE *datafile;
    if ( (datafile = fopen(argv[2],"r")) != NULL ) {

      /* データがランダムに生成するための乱数種を初期化しておく */
      srand((unsigned int)time(NULL));

      set_object_data(objnum, objects, datafile); // ファイルによる初期化
    }
    else{
      fprintf(stderr,"cannot open file %s\n",argv[1]);
      return EXIT_FAILURE;
    }
    fclose(datafile);
  }
  else{
    // objects[1] は巨大な物体を画面外に... 地球のようなものを想定
    objects[0] = (Object){ .m = 60.0, .y = -19.9, .x = 0.0, .vy = 2.0, .vx = 2.0}; 
    objects[1] = (Object){ .m = 100000.0, .y =  1000.0, .x = 0.0, .vy = 0.0, .vx = 0.0};
  }

  // シミュレーション. ループは整数で回しつつ、実数時間も更新する
  const double stop_time = 400;
  double t = 0;
  for (size_t i = 0 ; t <= stop_time ; i++){
    t = i * cond.dt;
    my_update_velocities(objects, objnum, cond);
    my_update_positions(objects, objnum, cond);
    my_bounce(objects, objnum, cond); // 壁があると仮定した場合に壁を跨いでいたら反射させる
    
    // 表示の座標系は width/2, height/2 のピクセル位置が原点となるようにする
    my_plot_objects(objects, objnum, t, cond);
    
    usleep(200 * 1000); // 200 x 1000us = 200 ms ずつ停止
    printf("\e[%dA", cond.height+3);// 壁とパラメータ表示分で3行
  }
  return EXIT_SUCCESS;
}

// 実習: 以下に my_ で始まる関数を実装する
// 最終的に 

double calc_distance(Object objs[], const size_t numobj, int i, int j){
 return sqrt(pow(objs[i].x - objs[j].x, 2) + pow(objs[i].y - objs[j].y, 2));
}

/* min～max 間の乱数を生成する  <- 不足の玉データを乱数で生成 */
double rand_data_gen(int min, int max){
  double i = (double)rand() / RAND_MAX;
  return min + i * (max - min);
}

void set_object_data(const int objnum, Object objs[objnum], FILE *datafile){
  char buf[256];
  int num = 0;
  while(fgets(buf, 256, datafile) != NULL && num < objnum){
    /* 最初のコメントを読み捨てる */
    if(buf[0] == '#') {
      buf[0] = '\0';
      continue;
    }
    sscanf(buf, "%lf %lf %lf %lf %lf", &objs[num].m, &objs[num].x, &objs[num].y, &objs[num].vx, &objs[num].vy);
    num++;
  }
  
  /* オブジェクト数よりdata.datデータが少ない場合は、質量が0のデータをランダムに生成 */
  while( num < objnum ){
    objs[num].m = 0;
    objs[num].x = rand_data_gen(-37,37);
    objs[num].y = rand_data_gen(-20,19);
    objs[num].vx = rand_data_gen(-2, 2);
    objs[num].vy = rand_data_gen(-2,2);
    num++;

  }
}

void my_plot_objects(Object objs[], const size_t numobj, const double t, const Condition cond){

  /* 上の壁　*/
  printf("+");
  for(int i = 0; i < cond.width; i++){
    printf("-");
  }
  printf("+\n");

  for (int i = 0; i < cond.height; i++){
    /* 現在の高度上に表示されている物体（前回の跡）をクリアする　*/
    printf("\e[K");
    /* カーソルを行頭に移動する　*/
    printf("\e[%dD",cond.width);
    printf("|");

    for(int j = 0; j < numobj; j++){

      if( i == (int)objs[j].y + cond.height / 2){
        if( objs[j].x > cond.width / (-2) && objs[j].x < cond.width / 2){
          printf("\e[%dC", (int)(objs[j].x + cond.width / 2 - 1));
          // printf("o");
          printf("\e[%dmo\e[0m",31+j);
        
          // 行頭へカーソルを移動する　*/
          printf("\r");
          // 行頭にある"|"の次へカーソルを移動する　*/
          printf("\e[1C");
        }
      }
    }
    /* カーソルを左へcond.width分を動かす */
    printf("\e[%dC", cond.width);
    
    /* 右の壁を描画する */
    printf("|\n");
  }

  /* 下の壁 */
  printf("+");
  for(int i = 0; i < cond.width; i++){
    printf("-");
  }
  printf("+\n");

  printf("t = %4.1f. ",t);
  for (int i = 0; i < numobj; i++){
    // printf("objs[%d].y = %.2f. ",i, objs[i].y);
    printf("objs[%d].y = %6.2f. objs[%d].x = %6.2f. ", i, objs[i].y, i, objs[i].x);
  }
  printf("\n");
}

void my_update_velocities(Object objs[], const size_t numobj, const Condition cond){
  float SumFx, SumFy;
  float F, Fx, Fy;
  float ax, ay;

  for(int i = 0; i < numobj; i++){
    SumFx = 0;
    SumFy = 0;

    for(int j = 0; j < numobj; j++){
      if ( j != i || !(((int)(objs[i].x - objs[j].x) == 0 ) && ((int)(objs[i].y - objs[j].y) == 0 ))){
          int distance = calc_distance(objs, numobj, i, j);
          if (distance < FUSION_DISTANCE) continue;
          F = cond.G * objs[i].m * objs[j].m / pow(distance, 2.0);
          Fx = F * (objs[j].x - objs[i].x) / distance;
          Fy = F * (objs[j].y - objs[i].y) / distance;
          SumFx += Fx;
          SumFy += Fy;
      }
    }
    if(objs[i].m != 0.0){
      ax = SumFx / objs[i].m;
      objs[i].prev_vx = objs[i].vx;
      objs[i].vx = objs[i].vx + ax * cond.dt;

      ay = SumFy / objs[i].m;
      objs[i].prev_vy = objs[i].vy;
      objs[i].vy = objs[i].vy + ay * cond.dt;
    }
  }
}

void my_update_positions(Object objs[], const size_t numobj, const Condition cond){
  for(int i = 0; i < numobj; i++){
    objs[i].prev_y = objs[i].y;
    objs[i].y = objs[i].y + objs[i].vy * cond.dt;
    objs[i].prev_x = objs[i].x;
    objs[i].x = objs[i].x + objs[i].vx * cond.dt;

  }
  /* ポジションを更新したあと、融合を処理 */
  collision_fusion(numobj, objs);
}

void my_bounce(Object objs[], const size_t numobj, const Condition cond){
  for(int i = 0; i < numobj; i++){
    /*  縦方向での跳ね返し処理　*/
    while((objs[i].y > cond.height / 2.0 && objs[i].prev_y < cond.height / 2.0) || (objs[i].y < cond.height / (-2.0) && objs[i].prev_y > cond.height/ (-2.0))){
      if(objs[i].y > cond.height / 2.0 && objs[i].prev_y < cond.height / 2.0){
        objs[i].y = cond.height - objs[i].y;
        objs[i].vy = (-1.0) * objs[i].vy * cond.cor;

      }else if (objs[i].y < cond.height / (-2.0) && objs[i].prev_y > cond.height/ (-2.0)){
        objs[i].y = (-1.0) * cond.height - objs[i].y;
        objs[i].vy = (-1.0)*objs[i].vy * cond.cor;
      }
    }
    /*  横方向での跳ね返し処理　*/
    while((objs[i].x > cond.width / 2.0 && objs[i].prev_x < cond.width / 2.0) || (objs[i].x < cond.width / (-2.0) && objs[i].prev_x > cond.width/ (-2.0))){
      if(objs[i].x > cond.width / 2.0 && objs[i].prev_x < cond.width / 2.0){
        objs[i].x = cond.width - objs[i].x;
        objs[i].vx = (-1.0) * objs[i].vx * cond.cor;

      }else if (objs[i].x < cond.width / (-2.0) && objs[i].prev_x > cond.width/ (-2.0)){
        objs[i].x = (-1.0) * cond.width - objs[i].x;
        objs[i].vx = (-1.0)*objs[i].vx * cond.cor;
      }
    }
  }
}

/* 融合を検知し、処理する */
void collision_fusion(const int numobj, Object objs[numobj]){

 /*　融合の要否について自身以降の全オブジェクトをチェックし、該当する場合は処理する */
  for(int i = 0; i < numobj - 1; i++){
    if ( objs[i].m != 0 ){

      /* 自身以降の全オブジェクトをチェック */
      for(int j = i + 1; j < numobj; j++){
        /* 融合する場合は、objs[i]に結果を格納し、objs[j]をクリアする */
        if(calc_distance(objs, numobj, i, j) < FUSION_DISTANCE && objs[j].m != 0 ){ 

          int m = objs[i].m + objs[j].m;

          objs[i].x = ( objs[i].x * objs[i].m + objs[j].x * objs[j].m ) / m;
          objs[i].prev_x = ( objs[i].prev_x * objs[i].m + objs[j].prev_x * objs[j].m ) / m;
          objs[i].y = ( objs[i].y * objs[i].m + objs[j].y * objs[j].m ) / m;
          objs[i].prev_y = ( objs[i].prev_y * objs[i].m + objs[j].prev_y * objs[j].m ) / m;

          /* 運動量保存則 */ 
          objs[i].vx = ( objs[i].vx * objs[i].m + objs[j].vx * objs[j].m ) / m;
          objs[i].vy = ( objs[i].vy * objs[i].m + objs[j].vy * objs[j].m ) / m;
          objs[i].m = m;

          /* 融合されるもの ojbs[j] を適当に描画エリア外において、質量や速度等を0にする */
          objs[j].x = 100;
          objs[j].prev_x = 100;
          objs[j].y = 100;
          objs[j].prev_y = 100;
          objs[j].vx = 0;
          objs[j].vy = 0;
          objs[j].m = 0;
        }
      }
    }
  }
}