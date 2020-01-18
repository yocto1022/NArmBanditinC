#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "mt19937ar.h"

//乱数マクロ
#define rand_int() (genrand_int32())
#define rand_double() (genrand_real3())

//シミュレーション設定
#define NumofArm 5								//腕の本数
#define NumofItr 1								//1エポックの試行回数
#define NumofSim 400							//エポック数
double rate[NumofArm] = {0.1,0.1,0.1,0.1,0.9};	//各腕の当たり確率


//関数群
int draw(int armIndex);				//スロットを引く
int epsilon_greedy(int simIndex);	//epsilon-greedyによる腕選択
int UCB(int simIndex);				//UCBによる腕選択

//アルゴリズムによらず必要になるもの
int numofchoised[NumofArm] = {};		//腕の選択回数
double estimatedReward[NumofArm] = {};	//推定平均報酬

//epsilon_greedy
#define epsilon 0.2

int main(){
	int itrIndex,simIndex;
	init_genrand((unsigned)time(NULL));
	int sumofReward=0.0;
	int instantReward;
	double averageReward;
	int act;
	int numofAction = 0;

	for(simIndex=0;simIndex<NumofSim;simIndex++){
		for(itrIndex=0;itrIndex<NumofItr;itrIndex++){
			//使うアルゴリズムによってコメントアウトを切り替え
//			act = epsilon_greedy(simIndex);
			act = UCB(simIndex);
			numofchoised[act]++;
			numofAction++;
			instantReward = draw(act);//腕によって瞬時報酬が変化
			estimatedReward[act] = ((((double)numofchoised[act]-1)/((double)numofchoised[act]))*estimatedReward[act]) + ((double)instantReward/(double)numofchoised[act]);//推定報酬を更新
			sumofReward += instantReward;
		}
		averageReward = (double)sumofReward / numofAction;

		printf("%d\t%f\n",simIndex,averageReward);//その試行までに得られた平均報酬
	}

	return 0;
}

int epsilon_greedy(int simIndex){
	int out;
	if(rand_double()>epsilon){
		//期待報酬が最大のものを選択
		int armIndex;
		double temp = 0;
		int argMaxofArm = 0;
		for(armIndex=0;armIndex<NumofArm;armIndex++){
			if(temp < estimatedReward[armIndex]){
				temp = estimatedReward[armIndex];
				argMaxofArm = armIndex;
			}
		}
		out = argMaxofArm;
	}else{
		//ランダムに選択
		out = rand_int() % NumofArm;
	}

	return out;
}

int UCB(int simIndex){
	int out=0;
	double maxUCB = 0;
	double tempUCB;
	int armIndex;

	for(armIndex=0;armIndex<NumofArm;armIndex++){
		tempUCB = estimatedReward[armIndex] + sqrt((2.0*log(simIndex))/(double)numofchoised[armIndex]);//UCBによる選択指標の計算
		if(tempUCB>maxUCB){
			out = armIndex;
			maxUCB = tempUCB;
		}
	}

	return out;
}

int draw(int armIndex){
	int out;
	if(rand_double()<rate[armIndex]){
		out = 1;
	}else{
		out = 0;
	}

	return out;
}