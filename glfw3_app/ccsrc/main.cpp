//=====================================================================//
/*!	@file
	@breif	バイナリーファイルを読み込んで、C 用データ羅列を作成
			Copyright (C) 2007, Hiramatsu Kunihito
	@author	平松邦仁( hira@rvf-rc45.net )
*/
//=====================================================================//
#include <windows.h>
#include <iostream>

using namespace std;

// タイトルの表示と簡単な説明
static void title(const char *cmd)
{
	char	*p, buff[256];

	p = strrchr(cmd, '\\');
	if(p != NULL) {
		strcpy(buff, p+1);
	} else {
		p = strrchr(cmd, '/');
		if(p != NULL) {
			strcpy(buff, p+1);
		} else {
			strcpy(buff, cmd);
		}
	}

	p = strrchr(buff, '.');
	if(p != NULL) *p = 0;

	printf("Create C-source data array\n");
	printf("Copyright (C) 2007, Hiramatsu Kunihito\n");
	printf("usage:\n");
	printf("	%s [options] file\n", buff);
	printf("	-header num        ヘッダーバイト\n");
	printf("	-length num        １行の数（標準１６バイト）\n");
	printf("\n");
}

//-----------------------------------------------------------------//
/*!
	@breif	main 関数
	@param[in]	argc	コマンド入力パラメーターの数	
	@param[in]	argv	コマンド入力パラメーターのリスト
	@return	常に０
*/
//-----------------------------------------------------------------//
int main(int argc, char *argv[])
{
	char*	fname = NULL;
	bool	opterr = false;
	bool	length = false;
	int		length_num = 16;
	bool	header = false;
	int		header_num = 0;

	for(int i = 1; i < argc; ++i) {
		char* p = argv[i];
		if(p[0] == '-') {
			if(strcmp(p, "-header")==0) header = true;
			else if(strcmp(p, "-length")==0) length = true;
			else opterr = true;
		} else {
			if(header == true) {
				sscanf(p, "%d", &header_num); 
				header = false;
			} else if(length == true) {
				sscanf(p, "%d", &length_num); 
				if(length_num < 0) length_num = 16;
				length = false;
			} else {
				fname = p;
			}
		}
	}

	if(opterr == true) {
		fprintf(stderr, "Illegual option...\n\n");
		title((const char *)argv[0]);
		return 1;
	}

	if(fname == NULL) {
		title((const char *)argv[0]);
		return 1;
	}

	FILE	*fp;

	fp = fopen(fname, "rb");
	if(fp == NULL) {
		fprintf(stderr, "Can't open input file: '%s'\n", fname);
		return 1;
	}

	int		len = 0;
	int		cnt = 0;
	int		c;
	while((c = fgetc(fp)) != EOF) {
		if(cnt < header_num) {
			printf("%d,", c);
			if(cnt == (header_num - 1)) {
				printf("\n");
			}
		} else {
			printf("0x%02X,", c);
			len++;
			if(len >= length_num) {
				len = 0;
				printf("\n");
			}
		}
		cnt++;
	}
	fclose(fp);

	if(len != 0) {
		printf("\n");
	}

	return 0;
}

/* ---- End Of File "main.cpp" ----- */
