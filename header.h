//1度だけインクルードされる
#ifndef MY
#define MY

#define MAXLINE 100 //行数
#define LINESIZE 40//1行のサイズ
#define MAX 4000 //MAXLINE * LINESIZE
#define MAXDATA 10000//形態素連鎖の最大数
#define DEFAULT_WARD "人工無能"
#define READ "read:"

typedef char baffa[MAX];

int GetRandom(int min, int max);

int IsEm(unsigned char c);

int IsPunc(char* c);

void GetStartText(char start[],char data[]);

void GetEm(char str[], char target[]);

int ReadMorph(char data[MAXDATA][2][LINESIZE]);

void ReadFileByLine(char file_name[],char result[MAXLINE][LINESIZE]);

int ReadFile(char (*file_name)[LINESIZE],baffa* result);

void OutPutMorph(char* source);

void WriteFile(char file_name[],char str[]);

#endif
