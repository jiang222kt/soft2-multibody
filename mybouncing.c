#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "physics2.h"

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
  Object objects[objnum];

  // objects[1] は巨大な物体を画面外に... 地球のようなものを想定
  objects[0] = (Object){ .m = 60.0, .y = -19.9, .vy = 2.0};
  objects[1] = (Object){ .m = 100000.0, .y =  1000.0, .vy = 0.0};

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
// 最終的に phisics2.h 内の事前に用意された関数プロトタイプをコメントアウト

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
        printf("\e[%dC", 37);
        printf("o");
        // printf("\e[%dmo\e[0m",31+j);
        
        // 行頭へカーソルを移動する　*/
        printf("\r");
        // 行頭にある"|"の次へカーソルを移動する　*/
        printf("\e[1C");
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
    printf("objs[%d].y = %6.2f.", i, objs[i].y);
  }
  printf("\n");
}

void my_update_velocities(Object objs[], const size_t numobj, const Condition cond){
  double ay;

  for(int i = 0; i < numobj; i++){
    ay = 0.0;

    for(int j = 0; j < numobj; j++){
      if ( j != i ){
          double r = abs(objs[j].y - objs[i].y);
          ay += (double)cond.G *objs[j].m *(objs[j].y - objs[i].y)/ (r* r* r);
      }
    }
    objs[i].prev_vy = objs[i].vy;
    objs[i].vy = objs[i].vy + ay * cond.dt;
  }
}

void my_update_positions(Object objs[], const size_t numobj, const Condition cond){
  for(int i = 0; i < numobj; i++){
    objs[i].prev_y = objs[i].y;
    objs[i].y += objs[i].prev_vy*cond.dt;
  }
}

void my_bounce(Object objs[], const size_t numobj, const Condition cond){
  
  for(int i = 0; i < numobj; i++){
    /*  縦方向での跳ね返し処理　*/
    if(objs[i].y > cond.height / 2 && objs[i].prev_y < cond.height / 2){
      objs[i].y = cond.height - objs[i].y;
      objs[i].vy = (-1.0) * objs[i].vy * cond.cor;

    }else if (objs[i].y < cond.height / (-2) && objs[i].prev_y > cond.height/ (-2)){
      objs[i].y = (-1) * cond.height - objs[i].y;
      objs[i].vy = (-1.0)*objs[i].vy * cond.cor;
    }
  }
}
