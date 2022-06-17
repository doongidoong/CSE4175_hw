#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int getwordCnt(char *arg,int dw_size){
	FILE *input;
	input= fopen(arg, "rb");// input 
	fseek(input, 0, SEEK_END);    // 파일 포인터를 파일의 끝으로 이동시킴
	int Fsize = ftell(input);          // 파일 포인터의 현재 위치를 얻음
	fclose(input);
	return Fsize;
	//return dw_size % 8 ? Fsize*2 : Fsize;
}

void makeBinary(char input, char *data){
	char temp;
	for (int i = 7; i > -1; i--) {
        temp = input & 1;
		data[i] = temp;
		input = input>>1;
    }
}

int makeByte(char *cw_tot ,int now){
	int	temp=0;
	for (int i = 0; i < 7; i++) {
		temp += cw_tot[now++];
		temp *= 2;
	}
	temp += cw_tot[now];
	return temp;
}
void modulo2(char *data, char *generator,int dt_size, int gn_size)
{   for (int i = 0; i <= dt_size - gn_size; i++) {
		if (data[i]==0)continue;
		for (int j = 0; j < gn_size; j++){
            if (data[i + j] == generator[j])
                data[i + j] = 0;
            else
                data[i + j] = 1;
        }
	}
}

int main(int argc, char *argv[]) {
	FILE *input_fp, *output_fp;
	input_fp = fopen(argv[1], "rb");// input 
	output_fp = fopen(argv[2], "wb"); //output
	//총 word의 개수 세기
	
	/*checking input error*/
	if(argc!=5){//받는 인자가 다섯개가 아니라면 error 
		fprintf(stderr, "usage: ./crc_encoder input_file output_file generator dataword_size\n");
		return 1;
	}
	if (input_fp == NULL){
		fprintf(stderr, "input file open error.\n");
		return 1;
	}
	if (output_fp == NULL){
		fprintf(stderr, "output file open error.\n");
		return 1;
	}
	int dw_size;
	dw_size= *argv[4]-'0';
	if(dw_size != 4 && dw_size != 8){
		fprintf(stderr, "dataword size must be 4 or 8.\n");
	 	return 1;
	}
	int word_cnt = getwordCnt(argv[1],dw_size);
	//printf("word cnt= %d \n",word_cnt);
	int data_cnt = dw_size % 8 ? word_cnt*2 : word_cnt;
	//printf("data cnt= %d \n",data_cnt);
	int gn_size = strlen(argv[3]);
	char generator[gn_size]; //generator 배열 생성
	for(int i=0;i<gn_size;i++){
		generator[i]= argv[3][i]-'0';
	}
	
	char *dw_tot;
	dw_tot = (char*)malloc(sizeof(char)*(data_cnt*dw_size));
	
	char word;
	char binary_dt[8];
	int cw_size = dw_size+gn_size-1;
	char *cw_tot;
	int padding;//padding을 구해준다

	padding = (8-(data_cnt*cw_size%8))%8;
	cw_tot= (char*)calloc(sizeof(char), (data_cnt*cw_size)+padding);
	int now= padding;
	for(int j=0;j<word_cnt;j++){
		fread(&word, 1, 1, input_fp); //처음받을때 무조건 1byte
		makeBinary(word,binary_dt);
		for(int i=0;i<8;i++){
			dw_tot[j*8+i]=binary_dt[i]; 
			// printf(" %d",binary_dt[i]);
		}
		if(dw_size==4){
			for(int i=0;i<2;i++){
				char *codeword= (char*)calloc(sizeof(char), cw_size); //codeword 생성 및 복사
				for(int k=0;k<4;k++){
					cw_tot[now++] = binary_dt[k+4*i]; //dataword가 4개이므로 먼저 넣고
					codeword[k] = binary_dt[k+4*i]; 
				}
				modulo2(codeword,generator, cw_size, gn_size);
				for(int k=4;k<cw_size;k++){
					cw_tot[now++] = codeword[k]; //modulo로 구한 나머지 부분을 넣는다
				}
				free(codeword);
			}
		}
		else{
			char *codeword= (char*)calloc(sizeof(char), cw_size);
			for(int k=0;k<8;k++){
					cw_tot[now++] = binary_dt[k];
					codeword[k] = binary_dt[k];
			}
			modulo2(codeword,generator, cw_size, gn_size);
			for(int k=8;k<cw_size;k++){
				cw_tot[now++] = codeword[k];
			}
			free(codeword);
		}
	}
	int rs_size = (data_cnt*cw_size+padding)/8 +1;
	char *result =(char *) malloc(sizeof(char) * rs_size);
	result[0]= padding;
	unsigned int temp;
	now=0;
	for(int i=1;i<rs_size;i++){
		temp= makeByte(cw_tot,now);
		result[i] = temp;
		now+=8;
	}
	// printf("\npadding = %d",padding);
	// printf("\nrs = %d",rs_size);
	// printf("\ndw = %d",dw_size);
	// printf("\ncw = %d",cw_size);
	// printf("\nwordcnt = %d",word_cnt);
	// printf("\ndatacnt = %d",data_cnt);
	
	fwrite(result, rs_size, 1, output_fp);
	free(result);
	fclose(input_fp);
	fclose(output_fp);
	free(dw_tot);
	free(cw_tot);
	return 0;
}

