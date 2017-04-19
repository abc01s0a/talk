#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

//開始文字が何回含まれるか数える
int FindChar(char* current, char db[MAXDATA][2][LINESIZE], int length) {
	int i;
	int result = 0;

	for (i = 0; i < length; i++) {
		//currentとデータが等しければカウントする
		if (strncmp(current, db[i][0], LINESIZE) == 0) ++result;
	}
	return result;
}

//次の文字をランダムにセットする
void SetRandomText(char* current, char db[MAXDATA][2][LINESIZE], int length) {
	int point = GetRandom(0, length - 1);

	//ランダム文字列をcurrentに設定
	strcpy_s(current,LINESIZE,db[point][1]);
}

//次の文字を連鎖でセット num:開始文字の個数
void SetNext(char* start, char db[MAXDATA][2][LINESIZE], int length, int num) {
	int i;
	int no = -1;
	int point = GetRandom(0, num - 1);//開始文字列のインデックスをランダムに作成

	//lengthの個数だけループ
	for (i = 0; i < length; i++) {
		//カレント文字列に等しければnoを+1
		if (strncmp(start, db[i][0],LINESIZE) == 0) ++no;
		//開始文字のインデックス
		if (no == point) {
			strcpy_s(start,LINESIZE, db[i][1]);
			break;
		}
	}

}

//文の生成
void Generates(char* current, char db[MAXDATA][2][LINESIZE], int length) {
	int num;
	printf("%s",current);

	//句点がでるまで繰り返す
	do {
		//currentと同じ文字列が含まれる数
		num = FindChar(current, db, length);

		if (num != 0) {
			//currentと同じ文字列を1つ以上発見していれば連鎖
			SetNext(current, db, length, num);
		} else {
			//currentと同じ文字列が無ければランダムに選ぶ（連鎖不可）
			SetRandomText(current, db, length);
		}
		//文字の出力
		printf("%s",current);

	} while ((strncmp(current, "．", 2) != 0) && (strncmp(current, "。", 2)) != 0);
  
}

//形態素ファイルの更新
void AddMorph(char* line) {
	baffa data;
	
	GetEm(line, data);//全角文字を取り出す
	OutPutMorph(data);//形態素を切り出す(ファイルに書き込む)
}


void main(void) {
	
	char db[MAXDATA][2][LINESIZE];
	char line[LINESIZE];
	int morph_n;//形態素の個数
	char current[LINESIZE];//現在の文字列
	int flag = 0;//飛ばすフラグ
	printf("Please Enter Message..\n・全角で入力\n・末尾に句読点\n・文字は全角%d文字まで\n",(LINESIZE/ 2)-1);

	//テキスト入力を受け付ける
	while (fgets(line,LINESIZE,stdin) != NULL) {
		if (flag == 1 || line[0] == '\n') {
			flag = 0;
			continue;
		}

		//末尾が改行でなければ途切れている
		if ('\n' != line[strlen(line)-1]) {
			flag = 1;
			printf("\n<--文字数がオーバーしています。-->\n");
			continue;
		}
		
		//最終文字が句読点でなければエラー
		if (IsPunc(&(line[strlen(line)-3])) == 0) {
			printf("\n<--句読点が発見できませんでした。>\n");
			continue;
		}

		//入力により形態素ファイルを更新
		AddMorph(line);
		//形態素ファイルを読み込む
		morph_n = ReadMorph(db);
		
		printf("Argent: ");
		GetStartText(current,line);
		Generates(current,db,morph_n);
		printf("->\n");
	}

	return;
}
