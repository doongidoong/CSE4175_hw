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
{   int i, j;
	for (int i = 0; i <= dt_size - gn_size; i++) {
		if (data[i]==0)continue;
		for (j = 0; j < gn_size; j++){
            if (data[i + j] == generator[j])
                data[i + j] = 0;
            else
                data[i + j] = 1;
        }
	}
}
int main(int argc, char *argv[]) {
	FILE *input_fp, *output_fp, *result_fp;
	input_fp = fopen(argv[1], "rb"); 
	output_fp = fopen(argv[2], "wb");
	result_fp = fopen(argv[3], "w");
	/*checking input error*/
	if(argc!=6){//받는 인자가 여섯개가 아니라면 error 
		fprintf(stderr, "usage: ./crc_decoder input_file output_file result_file generator dataword_size\n");
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
    
	if (result_fp == NULL){
		fprintf(stderr, "result file open error.\n");
		return 1;
	}
    
    int gn_size = strlen(argv[4]);
	char generator[gn_size]; //generator 배열 생성
	for(int i=0;i<gn_size;i++){
		generator[i]= argv[4][i]-'0';
	}
    char dw_size;
	dw_size= *argv[5]-'0';
	if(dw_size != 4 && dw_size != 8){
		fprintf(stderr, "dataword size must be 4 or 8.\n");
	 	return 1;
	}
    int word_cnt = getwordCnt(argv[1],dw_size); // 파일에 존재하는 word의 개수 
    //int data_cnt = dw_size % 8 ? word_cnt*2 : word_cnt;
	int tot_len = (word_cnt-1)*8; //패딩의 개수를 알려주는 첫번째 data 제외 전체 길이
	char *cw_tot = (char*)malloc(sizeof(char)*(tot_len)); //padding을 포함한 전체 word를 담는 공간
	char padding, word;
	char binary_dt[8];
	fread(&padding, 1, 1, input_fp);//일단 패딩 정보 읽어들임
	for(int j=0;j<word_cnt-1;j++){
		fread(&word, 1, 1, input_fp);
		makeBinary(word,binary_dt);
		for(int i=0;i<8;i++){
			cw_tot[j*8+i]=binary_dt[i]; 
		}
	}
	
	int cw_size = dw_size+gn_size-1;//codeword size
	int cw_cnt= (tot_len-padding)/cw_size ;//총 길이에서 codeword의 개수를 센다
	char *dw_tot = (char*)malloc(sizeof(char)*(dw_size)*cw_cnt); //dataword를 담을 공간
	int now= padding;//패딩을 제외하고 dataword로 변환한다
	int j;
	int count;
	unsigned char err;
	for(int i=0;i<cw_cnt;i++){
		count=0;
		char *codeword= (char*)calloc(sizeof(char), cw_size); 
		for(j=0;j<dw_size;j++){
			codeword[j] =cw_tot[now];
			dw_tot[(dw_size*i)+j] = cw_tot[now++];
			}
		for(;j<cw_size;j++){
			codeword[j] =cw_tot[now++];
		}
		modulo2(codeword,generator, cw_size, gn_size);
		for(int i=0;i<cw_size;i++){
			count+=codeword[i];
		}
		if(count!=0){
			err++;
		}
	}
	
	int sr_size = ((cw_cnt*dw_size)+padding)/8;
	char *stream =(char *) malloc(sizeof(char) * sr_size);
	unsigned int temp;
	now=0;
	for(int i=0;i<sr_size;i++){
		temp= makeByte(dw_tot,now);
		stream[i] = temp;
		now+=8;
	}
	fwrite(stream, sr_size, 1, output_fp);
	fprintf(result_fp,"%d %d",cw_cnt,err);
	
	fclose(input_fp);
	fclose(output_fp);
	fclose(result_fp);
	free(stream);
	free(dw_tot);
	free(cw_tot);
    return 0;
}