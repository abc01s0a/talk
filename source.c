#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "header.h"

//全角文字であるとき1、そうでなければ0を返す
int IsEm(unsigned char c) {
	int d = c;
	if (((d > 0x7F) && (d < 0xA0)) || (d > 0xDF) && (d < 0xF0)) return 1;
	return 0;
}

//ファイルを読み込む 成功1
int ReadFile(char (*file_name)[LINESIZE],baffa* result) {
	FILE* fp;
	char line[LINESIZE];

	//ファイルが存在しない
	if (fopen_s(&fp,*file_name,"r") != 0) return 0;

	//文字列に値が定義されていないとstrcat_sが失敗する
	if (fgets(*result, LINESIZE, fp) == NULL) return 0;

	//文字列を読み込む
	int i;
	for (i = 1; i < MAXLINE; i++) {
		if (fgets(line, LINESIZE, fp) == NULL) break;
		//++末尾に句読点がなければスキップ
		strcat_s(*result,_countof(*result),line);
	}
	
	fclose(fp);
	return 1;
}

//ファイルに書き込む
void WriteFile(char file_name[],char str[]) {
	FILE* fp;
	if ((fopen_s(&fp, file_name,"w")) != 0) {
		printf("file no open");
		exit(EXIT_FAILURE);
	}

	fputs(str,fp);

	fclose(fp);
}

//ファイルを読み込む 行毎に読み込んでstring配列化する
void ReadFileByLine(char file_name[],char lines[MAXLINE][LINESIZE]) {
	FILE* fp;
	if ((fopen_s(&fp,file_name,"r")) != 0) {
		printf("No File : %s\n",file_name);
		exit(EXIT_FAILURE);
	}
	
	int i;

	for(i=0;i<MAXLINE;i++){
		if (fgets(lines[i], LINESIZE, fp) == NULL) break;
		printf("%d[%d文字]: %s\n",i,strlen(lines[i]),lines[i]);
	}

	fclose(fp);
}



//全角文字のみ取り出す 改行も飛ばす
void GetEm(char str[],char target[]) {
	int in = 0;
	int out = 0;

	while (str[in] != '\0'){
		if (IsEm((unsigned char)str[in]) == 1) {
			target[out++] = str[in++];
			target[out++] = str[in++];
		} else {
			++in;
		}
	}
	target[out] = '\0';
}



//形態素ファイルを読み込む
int ReadMorph(char db[MAXDATA][2][LINESIZE]) {
	FILE* fp;
	char newline[LINESIZE], oldline[LINESIZE];
	int i = 0;

	if ((fopen_s(&fp,"morph.txt","r")) != 0) {
		printf("file not open : morph.txt \n");
		exit(EXIT_FAILURE);
	}
	
	//改行が発見できれば、それをEOSにする
	fgets(oldline,LINESIZE,fp);
	if (strchr(oldline,'\n') != NULL) {
		*strchr(oldline, '\n') = '\0';
	}
	
	while (fgets(newline, MAXLINE, fp) != NULL) {
		//改行を発見できればそれをEOSにする
		if (strchr(newline,'\n')!=NULL) {
			*strchr(newline, '\n') = '\0';
		}
		strncpy_s(db[i][0],LINESIZE,oldline,LINESIZE);
		strncpy_s(db[i][1],LINESIZE,newline,LINESIZE);
		strncpy_s(oldline,LINESIZE,newline,LINESIZE);//oldlineの更新

		++i;
		if (i >= MAXDATA) {
			printf("形態素数を%d個に制限する", MAXDATA);
			break;
		}
	}
	
	fclose(fp);
	return i;//形態素の個数を返す
}

//乱数
int GetRandom(int min, int max) {
	static int flag;
	if (flag != 1) {
		srand((unsigned int)time(NULL));
		flag = 1;
	}
	return min + (int)(rand() * (max - min + 1.0) / (1.0 + RAND_MAX));
}

//漢字であるか true=1.false=0
int IsKanji(char ch) {
	if ( ((unsigned char)ch) >= 0x88) return 1;
	else return 0;
}

//カタカナであるか 1=true,0=false
int IsKatakana(char ch1, char ch2) {
	int d1 = (unsigned char)ch1;
	int d2 = (unsigned char)ch2;

	if ((d1 == 0x83) && (d2 >= 40) && (d2 <= 0x96)) return 1;
	else return 0;
}

//字種の設定
int TypeSet(char ch1, char ch2) {
	if (IsKanji(ch1)) return 0;//漢字
	else if (IsKatakana(ch1, ch2)) return 1;//カタカナ
	else return 2;//その他
}

//句読点であるか
int IsPunc(char* ch) {
	if ((strncmp(ch, ".", 2) == 0)
		||(strncmp(ch,"。",2) == 0)
		||(strncmp(ch,"，",2) == 0)
		||(strncmp(ch,"、",2) == 0)
	)return 1;//句読点
	else return 0;
}

//開始文字列を返す
void GetStartText(char start[], char data[]) {
	int in,out;
	in = 0;
	out = 0;
	
	while (data[in] != '\0' && in < LINESIZE-1) {
		//全角文字であれば
		if (IsEm((unsigned char)data[in]) == 1) {
			//漢字であれば
			if (IsKanji(data[in]) == 1) {
				start[out++] = data[in];
				start[out++] = data[in+1];
			
			//漢字が終われば
			} else if(out != 0){
				break;
			}
			in++; in++;

		} else {//半角文字
			if (out != 0) break;
			in++;
		}
	}

	if (out != 0) {
		start[out] = '\0';
	} else {
		//漢字が発見できなければデフォルトキーワードを開始文字列にする
		strcpy_s(start,LINESIZE, DEFAULT_WARD);
	}
}

//形態素を切り出し、ファイルに書き込む
void OutPutMorph(char* source) {
	FILE* fp;
	if ((fopen_s(&fp, "morph.txt", "a")) != 0) {
		printf("not create a morph file");
		exit(EXIT_FAILURE);
	}

	int i = 0;
	int now, last;//字種 0:漢字 1:カタカナ 2:その他
	
	last = TypeSet(source[i],source[i+1]);

	while (source[i] != '\0') {
		//2要素毎(全角1文字毎)に処理
		
		//句読点でない
		if (IsPunc(&(source[i])) == 0) {
			//文内の処理
			now = TypeSet(source[i],source[i+1]);
			//字種が変わると区切る
			if (now != last) {
				fputc('\n',fp);
				last = now;//更新
			}
			//出力してカウントを次の文字に進める
			fputc(source[i++], fp);
			fputc(source[i++], fp);

		}
		else {//句読点であるとき単独行にする
		    //文末等
			fputc('\n', fp);//区切りの改行を出力
			fputc(source[i++], fp);
			fputc(source[i++], fp);
			fputc('\n', fp);
			last = TypeSet(source[i], source[i + 1]);
		}

	}

	fclose(fp);
	
}
