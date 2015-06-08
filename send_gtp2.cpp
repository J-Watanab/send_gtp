// GTPで通信するだけのサンプル
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <math.h>
#include <algorithm>
#include <functional>
using namespace std;
typedef double value_type; /* ソートするキーの型 */
double komi = 7.5;
const int B_SIZE    = 9;			// 碁盤の大きさ
const int WIDTH     = B_SIZE + 2;	// 枠を含めた横幅
const int BOARD_MAX = WIDTH * WIDTH;

const int PATTERN = 65536*2+(B_SIZE + 1)/2+(2*B_SIZE-1);
const int line_num = 40000;
const int REN_MAX = B_SIZE*B_SIZE;
int tesuu = 0;
int ko[1];
int kifu[500];
float weight[PATTERN];
int ishi_list[BOARD_MAX];//石の連結リスト
int ishi_ren[BOARD_MAX];//その座標にある石の連番号
int ren_max = 0;//連番号の最大値(０は連がひとつも存在しないことを表す)
int pw_num[17] = {40,96,175,285,438,653,955,1376,1967,2793,3950,5570,7838,11013,15458,21680,30392}; 
int distance_feature(int tz);
void pattern_feature(int tz,int color,int is_take_stone);
int distance_premove_feature(int tz);
int search_board(int win_move,int color);
typedef struct strin {
  int kari_ren;
  int ishi_num;
  int col;
  int start;
  int end;
  int kokyu_num;//連のダメ数
  int exist;
  short kokyu_iti[229];
  int adjacent_num;
  int adjacent[30];
} STRING;
strin ren[361]={0};
int board[BOARD_MAX] = {
	3,3,3,3,3,3,3,3,3,3,3,
	3,0,0,0,0,0,0,0,0,0,3,
	3,0,0,0,0,0,0,0,0,0,3,
	3,0,0,0,0,0,0,0,0,0,3,
	3,0,0,0,0,0,0,0,0,0,3,
	3,0,0,0,0,0,0,0,0,0,3,
	3,0,0,0,0,0,0,0,0,0,3,
	3,0,0,0,0,0,0,0,0,0,3,
	3,0,0,0,0,0,0,0,0,0,3,
	3,0,0,0,0,0,0,0,0,0,3,
	3,3,3,3,3,3,3,3,3,3,3
};
int board_sub[BOARD_MAX] = {
	3,3,3,3,3,3,3,3,3,3,3,
	3,0,0,0,0,0,0,0,0,0,3,
	3,0,0,0,0,0,0,0,0,0,3,
	3,0,0,0,0,0,0,0,0,0,3,
	3,0,0,0,0,0,0,0,0,0,3,
	3,0,0,0,0,0,0,0,0,0,3,
	3,0,0,0,0,0,0,0,0,0,3,
	3,0,0,0,0,0,0,0,0,0,3,
	3,0,0,0,0,0,0,0,0,0,3,
	3,0,0,0,0,0,0,0,0,0,3,
	3,3,3,3,3,3,3,3,3,3,3
};
int ren_num[BOARD_MAX] = {
	0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,
};
int ren_sub[BOARD_MAX] = {
	3,3,3,3,3,3,3,3,3,3,3,
	3,0,0,0,0,0,0,0,0,0,3,
	3,0,0,0,0,0,0,0,0,0,3,
	3,0,0,0,0,0,0,0,0,0,3,
	3,0,0,0,0,0,0,0,0,0,3,
	3,0,0,0,0,0,0,0,0,0,3,
	3,0,0,0,0,0,0,0,0,0,3,
	3,0,0,0,0,0,0,0,0,0,3,
	3,0,0,0,0,0,0,0,0,0,3,
	3,0,0,0,0,0,0,0,0,0,3,
	3,3,3,3,3,3,3,3,3,3,3
};
int initial_kokyu_num[BOARD_MAX] = {
	3,3,3,3,3,3,3,3,3,3,3,
	3,2,3,3,3,3,3,3,3,2,3,
	3,3,0,0,0,0,0,0,0,3,3,
	3,3,0,0,0,0,0,0,0,3,3,
	3,3,0,0,0,0,0,0,0,3,3,
	3,3,0,0,0,0,0,0,0,3,3,
	3,3,0,0,0,0,0,0,0,3,3,
	3,3,0,0,0,0,0,0,0,3,3,
	3,3,0,0,0,0,0,0,0,3,3,
	3,2,3,3,3,3,3,3,3,2,3,
	3,3,3,3,3,3,3,3,3,3,3
};
int board_pattern[BOARD_MAX] = {
	3,3,3,3,3,3,3,3,3,3,3,
	3,4115,275,275,275,275,275,275,275,4115,3,
	3,275,23,23,23,23,23,23,23,275,3,
	3,275,23,23,23,23,23,23,23,275,3,
	3,275,23,23,23,23,23,23,23,275,3,
	3,275,23,23,23,23,23,23,23,275,3,
	3,275,23,23,23,23,23,23,23,275,3,
	3,275,23,23,23,23,23,23,23,275,3,
	3,275,23,23,23,23,23,23,23,275,3,
	3,4115,275,275,275,275,275,275,275,4115,3,
	3,3,3,3,3,3,3,3,3,3,3
};
int board_pattern_sub[BOARD_MAX] = {
	3,3,3,3,3,3,3,3,3,3,3,
	3,4115,275,275,275,275,275,275,275,4115,3,
	3,275,23,23,23,23,23,23,23,275,3,
	3,275,23,23,23,23,23,23,23,275,3,
	3,275,23,23,23,23,23,23,23,275,3,
	3,275,23,23,23,23,23,23,23,275,3,
	3,275,23,23,23,23,23,23,23,275,3,
	3,275,23,23,23,23,23,23,23,275,3,
	3,275,23,23,23,23,23,23,23,275,3,
	3,4115,275,275,275,275,275,275,275,4115,3,
	3,3,3,3,3,3,3,3,3,3,3
};

int board_distance[BOARD_MAX] = {
	3,3,3,3,3,3,3,3,3,3,3,
	3,1,1,1,1,1,1,1,1,1,3,
	3,1,2,2,2,2,2,2,2,1,3,
	3,1,2,3,3,3,3,3,2,1,3,
	3,1,2,3,4,4,4,3,2,1,3,
	3,1,2,3,4,5,4,3,2,1,3,
	3,1,2,3,4,4,4,3,2,1,3,
	3,1,2,3,3,3,3,3,2,1,3,
	3,1,2,2,2,2,2,2,2,1,3,
	3,1,1,1,1,1,1,1,1,1,3,
	3,3,3,3,3,3,3,3,3,3,3
};

int dir4[4] = { +1,-1,+WIDTH,-WIDTH };	// 右、左、下、上への移動量

int hama[2];
void update_string(int tz,int color)
{
  int ren_num_friend[4] = {0,0,0,0};
  //int ren_num_enemy[4] = {0,0,0,0};
  int a,b;
  a=b=0;
  /*周囲の座標に味方の連があるか確認*/
  for(int i=0;i<4;i++){
    if((ren_num[tz+dir4[i]]!=0)&&(board[tz+dir4[i]]==color)){
      ren_num_friend[a]=ren_num[tz+dir4[i]];
      a++;
    }
    else if((ren_num[tz+dir4[i]]!=0)&&(board[tz+dir4[i]]==3-color)){
      //ren_num_enemy[b]=ren_num[tz+dir4[i]];
      b++;
    }
  }
  /*周囲に連が存在しない場合*/
  if((a+b)==0){
    ren[ren_max+1].kari_ren=ren_max+1;
    ren[ren_max+1].ishi_num=1;
    ren[ren_max+1].col=color;
    ren[ren_max+1].kokyu_num=initial_kokyu_num[tz];
  }
  /*周囲に敵連しか存在しない場合*/
  else if(a==0&&b!=0){
    ren[ren_max+1].kari_ren=ren_max+1;
    ren[ren_max+1].ishi_num=1;
    ren[ren_max+1].col=color;
    ren[ren_max+1].kokyu_num=initial_kokyu_num[tz]-b;
  }
  else{
    sort(ren_num_friend,ren_num_friend+a,greater<int>());
    for(int i=0;i<a;i++){
      ren[ren_num_friend[a]].kari_ren=ren_num_friend[0];//一番大きな連番号で仮連番号を更新
      ren[ren_num_friend[0]].ishi_num+=ren[ren_num_friend[a]].ishi_num;//繋がった連の分の石数を追加
      ren[ren_num_friend[0]].kokyu_num+=ren[ren_num_friend[a]].kokyu_num;//繋がった連の分のダメを追加
    }
    ren[ren_num_friend[0]].ishi_num+=1;//打った石の分も石数に追加
    ren[ren_num_friend[0]].kokyu_num-=b;
  }
}
value_type med3(value_type x, value_type y, value_type z)
/* x, y, z の中間値を返す */
{
    if (x < y)
        if (y < z) return y; else if (z < x) return x; else return z; else
        if (z < y) return y; else if (x < z) return x; else return z;
}
/*PW用のソート関数*/
void quicksort2(value_type a[][BOARD_MAX], int left, int right)
{
  if (left < right) {
        int i = left, j = right;
        value_type tmp, pivot = med3(a[1][i], a[1][i + (j - i) / 2], a[1][j]); /* (i+j)/2ではオーバーフローしてしまう */
        while (1) { /* a[] を pivot 以上と以下の集まりに分割する */
            while (a[1][i] > pivot) i++; /* a[i] >= pivot となる位置を検索 */
            while (pivot > a[1][j]) j--; /* a[j] <= pivot となる位置を検索 */
            if (i >= j) break;
            tmp = a[1][i]; a[1][i] = a[1][j]; a[1][j] = tmp; /* a[i],a[j] を交換 */
	    tmp = a[0][i]; a[0][i] = a[0][j]; a[0][j] = tmp; /* a[i],a[j] を交換 */	    
            i++; j--;
        }
	quicksort2(a, left, i - 1);  /* 分割した左を再帰的にソート */
	quicksort2(a, j + 1, right); /* 分割した右を再帰的にソート */
    }
}
int ko_z;
int all_playouts = 0;
int get_z(int x,int y)
{
	return (y+1)*WIDTH + (x+1);	// 0<= x <=8, 0<= y <=8
}
int get81(int z)
{
	if ( z==0 ) return 0;
	int y = z / WIDTH;	 	// 座標をx*10+yに変換。表示用。
	int x = z - y*WIDTH;	// 106 = 9*11 + 7 = (x,y)=(7,9) -> 79
	return x*100 + y;        // 19路ではx*100+y
}
int flip_color(int col) {
	return 3 - col;	// 石の色を反転させる
}

int check_board[BOARD_MAX];	// 検索済みフラグ用
int check_board2[BOARD_MAX];	// 検索済みフラグ用
// ダメと石数を数える再帰関数
// 4方向を調べて、空白だったら+1、自分の石なら再帰で。相手の石、壁ならそのまま。
void count_dame_sub(int tz, int color, int *p_dame, int *p_ishi)
{
  int z,i;

  check_board[tz] = 1;     // この位置(石)は検索済み
  (*p_ishi)++;             // 石の数
  for (i=0;i<4;i++) {
    z = tz + dir4[i];      // 4方向を調べる
    if ( check_board[z] ) continue;
    if ( board[z] == 0 ) {
      check_board[z] = 1;  // この位置(空点)はカウント済みに
      (*p_dame)++;         // ダメの数
    }
    if ( board[z] == color ) count_dame_sub(z, color, p_dame,p_ishi);  // 未探索の自分の石
  }
}
int count_dame_sub1(int tz, int color, int *p_dame)
{
  int z,i;
  
  check_board[tz] = 1;     // この位置(石)は検索済み
  //(*p_ishi)++;             // 石の数
  if(*p_dame>1)return 1;
  for (i=0;i<4;i++) {
    z = tz + dir4[i];      // 4方向を調べる
    if ( check_board[z] ) continue;
    if ( board[z] == 0 ) {
      check_board[z] = 1;  // この位置(空点)はカウント済みに
      (*p_dame)++;         // ダメの数
    }
    if ( board[z] == color ) return count_dame_sub1(z, color, p_dame);  // 未探索の自分の石
    return 0;
  }
}
int count_dame_sub2(int tz, int color, int *p_dame)
{
  int z,i;
  
  check_board[tz] = 1;     // この位置(石)は検索済み
  //(*p_ishi)++;             // 石の数
  if(*p_dame>2)return 1;
  for (i=0;i<4;i++) {
    z = tz + dir4[i];      // 4方向を調べる
    if ( check_board[z] ) continue;
    if ( board[z] == 0 ) {
      check_board[z] = 1;  // この位置(空点)はカウント済みに
      (*p_dame)++;         // ダメの数
    }
    if ( board[z] == color ) return count_dame_sub2(z, color, p_dame);  // 未探索の自分の石
    return 0;
  }
}

// 位置 tz におけるダメの数と石の数を計算。
void count_dame(int tz, int *p_dame, int *p_ishi)
{
  int i;
  *p_dame = *p_ishi = 0;
  for (i=0;i<BOARD_MAX;i++) check_board[i] = 0;
  count_dame_sub(tz, board[tz], p_dame, p_ishi);
}
void count_dame1(int tz, int *p_dame)
{
  int i;
  *p_dame = 0;
  memset(check_board,0,sizeof(check_board));
  count_dame_sub1(tz, board[tz], p_dame);
}
void count_dame2(int tz, int *p_dame)
{
  int i;
  *p_dame = 0;
  memset(check_board,0,sizeof(check_board));
  count_dame_sub2(tz, board[tz], p_dame);
}

// 石を消す
void kesu(int tz,int color)
{
	int z,i;
	
	board[tz] = 0;
	for (i=0;i<4;i++) {
		z = tz + dir4[i];
		if ( board[z] == color ) kesu(z,color);
	}
}
// 石を置く。エラーの時は0以外が返る
int move(int tz,int color,int *is_take_stone)
{
	if ( tz == 0 ) { ko[1] = 0; return 0; }	// パスの場合

	int around[4][3];	// 4方向のダメ数、石数、色
	int un_col = flip_color(color);	// 相手の石の色

	// 4方向の石のダメと石数を調べる
	int space = 0;			// 4方向の空白の数
	int kabe  = 0;			// 4方向の盤外の数
	int mikata_safe = 0;	// ダメ2以上で安全な味方の数
	int take_sum = 0;		// 取れる石の合計
	int ko_kamo = 0;		// コウになるかもしれない場所
	int i;
	for (i=0;i<4;i++) {
		around[i][0] = around[i][1] = around[i][2] = 0;
		int z = tz+dir4[i];
		int c = board[z];	// 石の色
		if ( c == 0 ) space++;
		if ( c == 3 ) kabe++;
		if ( c == 0 || c == 3 ) continue;
		int dame;	// ダメの数
		int ishi;	// 石の数
		count_dame(z, &dame, &ishi);
		around[i][0] = dame;
		around[i][1] = ishi;
		around[i][2] = c;
		if ( c == un_col && dame == 1 ) { take_sum += ishi; ko_kamo = z; }
		if ( c == color  && dame >= 2 ) mikata_safe++;
	}

	if ( take_sum == 0 && space == 0 && mikata_safe == 0 ) return 1; // 自殺手
	if ( tz == ko[1]                                      ) return 2; // コウ
	if ( kabe + mikata_safe == 4                         ) return 3; // 眼(ルール違反ではない)
	if ( board[tz] != 0                                  ) return 4; // 既に石がある

	for (i=0;i<4;i++) {
		int d = around[i][0];
		int n = around[i][1];
		int c = around[i][2];
		if ( c == un_col && d == 1 && board[tz+dir4[i]] ) {	// 石が取れる
		  is_take_stone++;
		  kesu(tz+dir4[i],un_col);
		  hama[color-1] += n;
		}
	}

	board[tz] = color;	// 石を置く

	int dame, ishi;
	count_dame(tz, &dame, &ishi);
	if ( take_sum == 1 && dame && ishi) ko[1] = ko_kamo;	// コウになる
	else ko[1] = 0;
	return 0;
}
void read_weight()
{
  FILE *fp;
   if ((fp = fopen("/home/junya/Documents/c++/weight9x9", "r")) == NULL) {
    fprintf(stderr, "%sのオープンに失敗しました.\n", "weight9x9");
    exit(EXIT_FAILURE);
  }
  int i=0;
  while( fscanf(fp,"%f",&weight[i]) != EOF ){
     i+=1;
  }
  fclose(fp);
  
}

int take_stone_feature_sub(int tz, int color)
{
    int z,i;
    int un_col = flip_color(color);
    int dame;
    check_board2[tz] = 1;     // この位置(石)は検索済み         
    for (i=0;i<4;i++) {
      z = tz + dir4[i];      // 4方向を調べる
      count_dame1(z,&dame);
      if ( check_board2[z] ) continue;
      if ( board[z] == un_col && dame ==1) return 100;
      else if ( board[z] == color ) {
	return take_stone_feature_sub(z,color);
      }
    }//i
}
int take_stone_feature(int tz,int color,int pre_move)
{
  if ( tz == 0 ) { return 0; }	// パスの場合

  int around[4][2];	// 4方向のダメ数、石数、色
  int un_col = flip_color(color);	// 相手の石の色

  // 4方向の石のダメと石数を調べる
	
  int i,j,k;
  for (i=0;i<4;i++) {
    int z = tz+dir4[i];
    int c = board[z];	// 石の色
		
    if ( c == 0 || c == 3 ) continue;
    int dame;	// ダメの数
    count_dame1(z, &dame);
  
  
    if ( c == un_col && dame == 1) {	// 相手の石が取れる
      /*直前の相手の石をとる*/
      if(pre_move==tz+dir4[i]){
	return PATTERN-1;
      }
      //memset(check_board2,0,sizeof(check_board2));
      /*アタリにされた周囲の敵連を取る*/
      //if(take_stone_feature_sub(tz+dir4[i],un_col)==100) return PATTERN-2 ;
      /*上以外のパターンの相手の石を取る*/
      return PATTERN-3;
    }
  }
  return 0;
}
int atari(int tz,int color,int pre_move)
{ 
  
  int un_col = flip_color(color);	// 相手の石の色

  // 4方向の石のダメと石数を調べる
  int i;
  for (i=0;i<4;i++) {
    int z = tz+dir4[i];
    int c = board[z];	// 石の色
    if ( c == 0 || c == 3 ) continue;
    int dame;	// ダメの数
    count_dame2(z, &dame);
    if ( c == un_col && dame == 1) {	// 相手の石が取れる
      /*直前の相手の石をとる*/
      if(pre_move==tz+dir4[i]){
	return PATTERN-1;
      }
      /*上以外のパターンの相手の石を取る*/
      return PATTERN-3;
    }
    /*アタリ*/
    if ( c == un_col && dame == 2 ) {
      if(ko!=0)return PATTERN-5;//コウがあるときのアタリ
      return PATTERN-7;//それ以外のアタリ
    }
    /*アタリから逃げる*/
    if( c == color && dame == 1 )return PATTERN-9;
    /*アタリに突っ込む*/
    board[tz]=color;
    count_dame1(tz, &dame);
    board[tz]=0;//ボードを元に戻す
    if(dame==1)return PATTERN-10;
  }
  
  return 0;
}
int distance_premove_feature(int tz,int pre_move){
  if(pre_move==0)return 1 + (B_SIZE+1)/2;
  int dx,dy;
  dx = abs((tz%WIDTH)-(pre_move%WIDTH));
  dy = abs((tz-tz%WIDTH)/WIDTH-(pre_move-pre_move%WIDTH)/WIDTH);
  
  return dx+dy+(B_SIZE+1)/2+1;
}
void pattern_feature(int tz,int color,int is_take_stone)
{
  int n_1,n_2;
  int r[9]={0,-(WIDTH+1),-WIDTH,-(WIDTH-1),1,WIDTH+1,WIDTH,WIDTH-1,-1};
  if(is_take_stone==0){
    for(int i = 0;i<9;i++){
      if(board[tz+r[i]]==3)break;
      n_1=0;n_2=100000;
      int right_left[2][8]={
	{-(WIDTH+1),-WIDTH,-(WIDTH-1),1,WIDTH+1,WIDTH,WIDTH-1,-1},
	{-(WIDTH+1),-1,WIDTH-1,WIDTH,WIDTH+1,1,-(WIDTH-1),-WIDTH},
      };
      for(int s=0;s<2;s++)//右回り、左回り
	{
	  for(int t=0;t<4;t++)//左上、右上、右下、左下
	    {
	      for(int k=0;k<8;k++){
		n_1 = n_1 << 2;	  
		n_1 += board[tz+r[i]+right_left[s][(2*t+k)%8]];
	    
	      }
	      if(n_2>=n_1)n_2 = n_1;
	      n_1 = 0;
	    }
	}
      board_pattern[tz+r[i]]=n_2 + (B_SIZE+1)/2 + B_SIZE*2 + (1-color)*65536;
    }
  }
  else{
    for(int i=1;i<BOARD_MAX;i++){
	if(board[i]==3)break;
	n_1=0;n_2=100000;
	int right_left[2][8]={
	  {-(WIDTH+1),-WIDTH,-(WIDTH-1),1,WIDTH+1,WIDTH,WIDTH-1,-1},
	  {-(WIDTH+1),-1,WIDTH-1,WIDTH,WIDTH+1,1,-(WIDTH-1),-WIDTH},
	};
	for(int s=0;s<2;s++)//右回り、左回り
	  {
	    for(int t=0;t<4;t++)//左上、右上、右下、左下
	      {
		for(int k=0;k<8;k++){
		  n_1 = n_1 << 2;	  
		  n_1 += board[i+right_left[s][(2*t+k)%8]];
	    
		}
		if(n_2>=n_1)n_2 = n_1;
		n_1 = 0;
	      }
	  }
	board_pattern[i]=n_2 + (B_SIZE+1)/2 + B_SIZE*2+ (1-color)*65536;
    }
  }
  //return n_2 + 10 + 37 + 1;
}

void print_board()
{
	int x,y;
	const char * str[3] = {" ."," x"," o"};

	printf("    ");
	for (x=0;x<B_SIZE;x++) printf("%d ",x+1);
	printf("\n");
	for (y=0;y<B_SIZE;y++) {
		printf("%d: ",y+1);
		for (x=0;x<B_SIZE;x++) {
			printf("%s",str[board[get_z(x,y)]]);
		}
		printf("\n");
	}
}
void print_string()
{
	int x,y;
	const char * str[3] = {" ."," x"," o"};

	printf("    ");
	for (x=0;x<B_SIZE;x++) printf("%d ",x+1);
	printf("\n");
	for (y=0;y<B_SIZE;y++) {
		printf("%d: ",y+1);
		for (x=0;x<B_SIZE;x++) {
			printf("%s",str[ren_num[get_z(x,y)]]);
		}
		printf("\n");
	}
}


int count_score(int turn_color)
{
  int score = 0;
  int kind[3];  // 盤上に残ってる石数
  kind[0] = kind[1] = kind[2] = 0;
  int x,y,i;
  for (y=0;y<B_SIZE;y++) for (x=0;x<B_SIZE;x++) {
    int z = get_z(x,y);
    int c = board[z];
    kind[c]++;
    if ( c != 0 ) continue;
	int mk[4];	// 空点は4方向の石を種類別に数える
	mk[1] = mk[2] = 0;  
	for (i=0;i<4;i++) mk[ board[z+dir4[i]] ]++;
	if ( mk[1] && mk[2]==0 ) score++; // 同色だけに囲まれていれば地
	if ( mk[2] && mk[1]==0 ) score--;
  }
  score += kind[1] - kind[2];
 
  double final_score = score - komi;
  int win = 0;
  if ( final_score > 0 ) win = 1;
//  printf("win=%d,score=%d\n",win,score);
//  win = score;

  if ( turn_color == 2 ) win = -win; 
  return win;
}



/*int select_best_move(int color)
{
	int try_num = 1000; // playoutを繰り返す回数
	int    best_z     =  0;
	double best_value = -100;

	int board_copy[BOARD_MAX];	// 現局面を保存
	memcpy(board_copy, board, sizeof(board));
	int ko_z_copy = ko_z;

	// すべての空点を着手候補に
	int x,y;
	for (y=0;y<B_SIZE;y++) for (x=0;x<B_SIZE;x++) {
		int z = get_z(x,y);
		if ( board[z] != 0 ) continue;

		int err = move(z,color);	// 打ってみる
//		if ( err != 0 ) continue;	// エラー

		int win_sum = 0;
		int i;
		for (i=0;i<try_num;i++) {
			int board_copy[BOARD_MAX];
			memcpy(board_copy, board, sizeof(board));
			int ko_z_copy = ko_z;

			int win = -playout(flip_color(color));
			win_sum += win;
//			print_board();
//			printf("win=%d,%d\n",win,win_sum);

			memcpy(board, board_copy, sizeof(board));
			ko_z = ko_z_copy;
		}
		double win_rate = (double)win_sum / try_num;
//		print_board();
		printf("z=%d,win=%5.3f\n",get81(z),win_rate);
		
		if ( win_rate > best_value ) {
			best_value = win_rate;
			best_z = z;
			printf("best_z=%d,v=%5.3f,try_num=%d\n",get81(best_z),best_value,try_num);
		}

		memcpy(board, board_copy, sizeof(board));  // 局面を戻す
		ko_z = ko_z_copy;
	}
	return best_z;
	}*/




typedef struct child {
  int z;       // 手の場所
  int games;   // この手を探索した回数
  double rate; // この手の勝率
  int next;    // この手を打ったあとのノード
} CHILD;

const int CHILD_MAX = B_SIZE*B_SIZE+1;  // +1はPASS用

typedef struct node {
  int child_num;          // 子局面の数
  CHILD child[CHILD_MAX];
  int games_sum;// playoutの総数
} NODE;

const int NODE_MAX = 100000;
NODE node[NODE_MAX];
int node_num = 0;          // 登録ノード数
const int NODE_EMPTY = -1; // 次のノードが存在しない場合
const int ILLEGAL_Z  = -1; // ルール違反の手
double all_sum;
double sum_soft;
double temp,r;
double sum[BOARD_MAX];
int select_high_weight(int kouho_num,int kouho[],int color,int pre_move){
  
  all_sum=sum_soft=0;
  memset(sum,0,sizeof(sum));
  for(int i=0;i<kouho_num;i++){
    //int t = take_stone_feature(kouho[i],color,pre_move);
    int a = atari(kouho[i],color,pre_move);
    sum[i] = exp(weight[board_pattern[kouho[i]]]+weight[board_distance[kouho[i]]]+weight[distance_premove_feature(kouho[i],pre_move)]);
    sum_soft += sum[i];
  }
  
  r = sum_soft * (double)(rand())/((double)(RAND_MAX) + 1.0);
  temp = 0;
  for(int i=0;i<kouho_num;i++){
    temp += sum[i];
    if( r < temp ){
      return i;
    }
  }
  
  return 0;  
}

int playout(int turn_color)
{
	all_playouts++;
	int color = turn_color;
	int before_z = 0;	// 1手前の手
	int loop;
	int loop_max = B_SIZE*B_SIZE + 200;	// 最大でも300手程度まで。3コウ対策
	for (loop=0; loop<loop_max; loop++) {
		// すべての空点を着手候補にする
		int kouho[BOARD_MAX];
		int kouho_num = 0;
		int x,y;
		for (y=0;y<B_SIZE;y++) { 
		  for (x=0;x<B_SIZE;x++) {
		    int z = get_z(x,y);
		    if ( board[z] != 0 ) continue;
		    kouho[kouho_num] = z;
		    kouho_num++;
		  }
		}
		int z,r = 0;
		for (;;) {
			if ( kouho_num == 0 ) {
				z = 0;
			} else {
			  //r = rand() % kouho_num;		// 乱数で1手選ぶ
			  r = select_high_weight(kouho_num,kouho,color,before_z);
			  z = kouho[r];
			}
			int is_take_stone_in_playout = 0;
			int err = move(z,color,&is_take_stone_in_playout);
			
			if ( err == 0 ) {
			  pattern_feature(z,color,is_take_stone_in_playout);
			  break;
			}
			kouho[r] = kouho[kouho_num-1];	// エラーなので削除
			kouho_num--;
		}
		if ( z == 0 && before_z == 0 ) break;	// 連続パス
		before_z = z;
//		print_board();
//		printf("loop=%d,z=%d,c=%d,kouho_num=%d,ko_z=%d\n",loop,get81(z),color,kouho_num,get81(ko_z));
		color = flip_color(color);
	}
	
	return count_score(turn_color);
}

void add_child(NODE *pN, int z)
{
  int n = pN->child_num;
  pN->child[n].z     = z;
  pN->child[n].games = 0;
  pN->child[n].rate  = 0;
  pN->child[n].next  = NODE_EMPTY;
  pN->child_num++;
}

// ノードを作成する。作成したノード番号を返す
//重みの大きいノードから追加
int create_node()
{
  if ( node_num == NODE_MAX ) { printf("node over Err\n"); exit(0); }
  NODE *pN = &node[node_num];
  pN->child_num = 0;
  int x,y,i;
  value_type node_weight[2][BOARD_MAX];//各手の重み
  i = 0;
  for (y=0;y<B_SIZE;y++) for (x=0;x<B_SIZE;x++) {
    int z = get_z(x,y);
    if ( board[z] != 0 ) continue;
    node_weight[0][i]=z;
    node_weight[1][i]=exp(weight[board_pattern[z]]+weight[board_distance[z]]/*+weight[distance_premove_feature(z,pre_move)]*/);
    //printf("i=%d pattern = %d d=%d w_p=%f w_d=%f %f\n",i,board_pattern[z],board_distance[z],weight[board_pattern[z]],weight[board_distance[z]],node_weight[1][i]);
    i++;
  }
  quicksort2(node_weight,0,i);
  for(int j = 0; j < i;j++){
    //    printf("j=%d %d %f\n",j,(int)node_weight[0][j],node_weight[1][j]);
    add_child(pN,(int)node_weight[0][j]);
  }
  /*for (y=0;y<B_SIZE;y++) for (x=0;x<B_SIZE;x++) {
    int z = get_z(x,y);
    if ( board[z] != 0 ) continue;
	add_child(pN, z);
	}*/
  
  add_child(pN, 0);  // PASSも追加

  node_num++;
  return node_num-1; 
}
int search_uct(int color, int node_n)
{
  NODE *pN = &node[node_n];
re_try:
  // UCBが一番高い手を選ぶ
  int select = -1;
  double max_ucb = -999;
  int i,j;
  j=1;
  for (i=0; i <17;i++){
    if(pw_num[i]>pN->games_sum)
      {
       break;
      }
    j++;
  }
  //printf("j=%d\n",j);
  for (i=0; i</*pN->child_num*/j; i++) {//上位j手に限定
    CHILD *c = &pN->child[i];
    if ( c->z == ILLEGAL_Z ) {
      j++;
      //printf("c->z=%d\n",c->z);      
      continue;
    }
    double ucb = 0;
    if ( c->games==0 ) {
      ucb = 10000 + rand();  // 未展開
    } else {
      const double C = 0.31;
      ucb = c->rate + C * sqrt( log(pN->games_sum) / c->games );
    }
    if ( ucb > max_ucb ) {
      max_ucb = ucb;
      select = i;
    }
    //printf("in i=%d c->games=%d select=%d\n",i,c->games,select);
  }
  //printf("select=%d\n",select);
  if ( select == -1 ) { printf("Err! select\n"); exit(0); }

  CHILD *c = &pN->child[select];
  int z = c->z;
  int is_take_stone = 0;
  int err = move(z,color,&is_take_stone);  // 打ってみる
  if ( err != 0 ) {  // エラー
    //printf("z=%d\n",z);
    //print_board();
    c->z = ILLEGAL_Z;
    goto re_try;     //	別な手を選ぶ
  }
  pattern_feature(z,color,is_take_stone);
  int win;
  if ( c->games == 0 ) {  // 最初の1回目はplayout
    win = -playout(flip_color(color));
  } else {
    if ( c->next == NODE_EMPTY ) c->next = create_node();
    win = -search_uct(flip_color(color), c->next);
  }
  // printf("win=%d\n",win);
  // 勝率を更新
  c->rate = (c->rate * c->games + win) / (c->games + 1);
  c->games++;		// この手の回数を更新
  pN->games_sum++;  // 全体の回数を更新

  return win;  
}


int uct_loop = 10000;  // uctでplayoutを行う回数

int select_best_uct(int color)
{
  node_num = 0;
  int next = create_node();

  int i;
  //int *board_copy = new int[BOARD_MAX]; 
  
  for (i=0; i<uct_loop; i++) {
    int ko_z_copy = ko[1];
    int board_copy[BOARD_MAX];
    int board_pattern_copy[BOARD_MAX];
    // printf("i=%d\n",i);
    memcpy(board_copy, board, sizeof(board));// 局面を保存
    memcpy(board_pattern_copy, board_pattern, sizeof(board));// 局面を保存
    search_uct(color, next);
    memcpy(board, board_copy, sizeof(board)); // 局面を戻す
    memcpy(board_pattern, board_pattern_copy, sizeof(board));// 局面を保存
    ko[1] = ko_z_copy;
  }
  //delete [] board_copy;
  int best_i = -1;
  int max = -999;
  NODE *pN = &node[next];
  for (i=0; i<pN->child_num; i++) {
    CHILD *c = &pN->child[i];
    if ( c->games > max ) {
      best_i = i;
      max = c->games;
    }
    //printf("%3d:z=%2d,games=%5d,rate=%.4f\n",i,get81(c->z),c->games,c->rate);
  }
  int ret_z = pN->child[best_i].z;
  
  
  //fprintf(stderr,"z=%2d,rate=%.4f,games=%d,playouts=%d,nodes=%d\n",get81(ret_z),pN->child[best_i].rate,max,all_playouts,node_num);
  return ret_z;
}

int search_board(int color) {
  srand( (unsigned)time( NULL ) );
 
  //print_board();
  
 re_try_in_search_board:
  int z = select_best_uct(color);
  int is_take_stone = 0;
  if(board[z]!=0)goto re_try_in_search_board;
  int err = move(z,color,&is_take_stone);
  if(err!=0)goto re_try_in_search_board;
  kifu[tesuu++] = z;
  //fprintf(stderr,"select %d\n",z);
  if(z==0 && tesuu>1)return -1;
  if(z!=0)pattern_feature(z,color,is_take_stone);
  
  return z;
}
void read_command(char *command){
  int color;
  if(command[5]=='B')color = 1;
  else color = 2;
  int z;
  if(command[7]=='P')z=0;
  else{
    int x,y;
    x = (unsigned int)command[7]-64;
    if(x==10)x=9;
    y = (unsigned int)command[8]-48;
    z = x+11*y;
    int is_take_stone = 0;
    //board[z]=color;
    int err = move(z,color,&is_take_stone);
    if(err==3)board[z]=color;//相手が自ら目に打ったときは石を置く
    pattern_feature(z,color,is_take_stone);
  }
  //  std::cout << z << ko[1] << '\n';
  kifu[tesuu++] = z;
  
}
void create_command(int color,char *buff){
  
  char x[9]={'A','B','C','D','E','F','G','H','J'};
  char y[9]={'1','2','3','4','5','6','7','8','9'};
  //std::cout << z << '\n' <<  color << '\n';
  int selected_move = 0;
  if(kifu[tesuu-1]==0&&tesuu>1)selected_move = -1;
  else selected_move = search_board(color);
  //std::cout << tesuu <<selected_move << '\n';
  //fprintf(stderr,"select %d %c %c\n",selected_move,x[(selected_move%WIDTH)-1],y[((selected_move-selected_move%WIDTH)/WIDTH)-1]);
  if(selected_move == -1)sprintf(buff,"PASS");
  else sprintf(buff,"%c%c",x[(selected_move%WIDTH)-1],y[((selected_move-selected_move%WIDTH)/WIDTH)-1]);
  //fprintf(stderr,"select %s\n",buff);
  //std::cout << x[(z%WIDTH)-1] <<'\n';
  //std::cout << y[((z-z%WIDTH)/WIDTH)-1] <<'\n';
  // std::cout << buff <<'\n';
}
void send_gtp(const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vfprintf( stdout, fmt, ap );  // 標準出力に書き出す
  va_end(ap);
}

int main()
{
  char str[256];
  char buff[256];
  read_weight();
  // stdoutをバッファリングしないように。GTPで通信に失敗するので。
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);  // stderrに書くとGoGuiに表示される。
  for (;;) {
    if ( fgets(str, 256, stdin)==NULL ) break;  // 標準入力から読む

    if      ( strstr(str,"boardsize")   ) {
      send_gtp("= \n\n");    // "boardsize 19" 19路盤
      // fprintf( stderr, "stderrに書くとgoguiに表示されます\n");
    }
    else if ( strstr(str,"clear_board") ) {
      tesuu=0;
      node_num = 0;
      memcpy(board, board_sub, sizeof(board));
      memcpy(board_pattern, board_pattern_sub, sizeof(board));
      memset(kifu,0,sizeof(kifu));
      memset(hama,0,sizeof(hama));
      send_gtp("= \n\n");
    }
    else if ( strstr(str,"name")        ) send_gtp("= my_test\n\n");
    else if ( strstr(str,"version")     ) send_gtp("= 0.0.1\n\n");
    else if ( strstr(str,"showboard")   ) {
      print_board();
      send_gtp("= \n\n");
    }
    else if ( strstr(str,"showstring")   ) {
      print_string();
      send_gtp("= \n\n");
    }
    else if ( strstr(str,"genmove w")   ) {
      //   std::cout << z << '\n';
      fprintf( stderr, "I'm thinking...%d\n",100);
      create_command(2,buff);
      send_gtp("= %s\n\n",buff);
      //send_gtp("= E4\n\n");  // 自分の手を送る。この例では "D4" に打つ
    }
    else if ( strstr(str,"genmove b")   ){
      create_command(1,buff);
      send_gtp("= %s\n\n",buff);
    }
    // 相手の手を受信 "play W D17" のように来る
    else if ( strstr(str,"play")        ){
      read_command(str);
      send_gtp("= \n\n");
    }
    else if ( strstr(str,"quit")        ){
      return 0;
    }
    else {
      send_gtp("= \n\n");  // それ以外のコマンドにはOKを返す
    }
  }
  return 0;
}
