#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INF 10001
typedef struct element{
	int direct;
	int cost;
	int next;
}element;
int node_num;
struct element rt_table[100][100];
char message[100][1001]={0,};
void print_table(FILE *output_fp){
	for(int a=0;a<node_num;a++){
		for(int i=0;i<node_num;i++){
			if(rt_table[a][i].cost!=INF)fprintf(output_fp,"%d %d %d\n",i,rt_table[a][i].next ,rt_table[a][i].cost);
		}
	fprintf(output_fp,"\n");}
}

int giveInfo(int i, int j){
	int dist = rt_table[j][i].direct;
	int flag = 0;
	for(int k=0;k<node_num;k++){
		if(rt_table[j][k].cost >dist + rt_table[i][k].cost){
			rt_table[j][k].cost = rt_table[i][k].cost+dist;
			rt_table[j][k].next = i;
			flag=1;
		}
		else if(rt_table[j][k].cost==dist + rt_table[i][k].cost){
			if(rt_table[j][k].next > i) {
				rt_table[j][k].next = i;
				flag =1;}
		}
	}
	return flag;
}
void transMsg(FILE *output_fp, int cnt){
	int start;
	int dest;
	int now;
	int temp;
	for(int i=0;i<cnt;i++){
		start= message[i][0]-'0';
		dest= message[i][2]-'0';
		if(rt_table[start][dest].cost==INF){
			fprintf(output_fp,"from %d to %d cost infinite hops unreachable ",start,dest);
			temp=4;	
		}
		else{
			fprintf(output_fp,"from %d to %d cost %d hops ",start,dest,rt_table[start][dest].cost);
			fprintf(output_fp,"%d ",start);
			now = rt_table[start][dest].next;
			while(now!=dest){
				fprintf(output_fp,"%d ",now);		
				now = rt_table[now][dest].next;
			}
			temp=4;
		}
		fprintf(output_fp,"message ");
		while(message[i][temp]!='\0'){
			fprintf(output_fp,"%c",message[i][temp++]);
		}
		
	}
	if(cnt) fprintf(output_fp,"\n");
	
};
void Routing(){
	int changed=1;
	while(changed){
		changed=0;
		for(int i=0;i<node_num;i++){
			for(int j=0;j<node_num;j++){
				if(rt_table[i][j].direct>0){
					changed+= giveInfo(i,j);
				}
			}
		}
	}
}

void input_Table(int a,int b, int c){
	rt_table[a][b].cost = c;
	rt_table[b][a].cost = c;
	rt_table[a][b].direct = c;
	rt_table[b][a].direct = c;
	rt_table[a][b].next = b;
	rt_table[b][a].next = a;
}

void get_change(int a, int b, int c){
	for(int i=0;i<node_num;i++){
		for(int j=0;j<node_num;j++){
			rt_table[i][j].cost=INF;
			rt_table[i][j].next=-1;
			if(rt_table[i][j].direct>0){
				rt_table[i][j].cost= rt_table[i][j].direct;
				rt_table[i][j].next=j;
			}
			else if(i==j){
				rt_table[i][j].cost=0;
				rt_table[i][j].next=j;				
			}
		}	
	}
	if(c==-999){
		rt_table[a][b].cost = INF;
		rt_table[b][a].cost = INF;
		rt_table[a][b].direct = -1;
		rt_table[b][a].direct = -1;
		rt_table[a][b].next = -1;
		rt_table[b][a].next = -1;
		
	}
	else{
		input_Table(a,b,c);
	}
}

int main(int argc, char *argv[]) {
	
	FILE *top_fp,*mes_fp, *cha_fp, *output_fp;
	top_fp = fopen(argv[1], "r");// input
	mes_fp = fopen(argv[2], "r");// input 
	cha_fp = fopen(argv[3], "r");// input 
	//총 word의 개수 세기
	
	/*checking input error*/
	if(argc!=4){//받는 인자가 네 개가 아니라면 error 
		fprintf(stderr, "usage: distvec topologyfile messagesfile changesfile\n");
		return 1;
	}
	if ((top_fp == NULL)||(mes_fp == NULL)||(cha_fp == NULL)){
		fprintf(stderr, "Error: open input file.\n");
		return 1;
	}
	output_fp = fopen("output_dv.txt", "w"); //output

	int node1, node2, cost; 
	fscanf(top_fp, "%d", &node_num);
	int i,j;
	for(i=0;i<node_num;i++){
		for(j=0;j<node_num;j++){
			if(i==j) {
				rt_table[i][j].cost=0;
				rt_table[i][j].next=j;
				continue;
			}
			rt_table[i][j].cost=INF;
			rt_table[i][j].cost=INF;
			rt_table[i][j].next=-1;		
			rt_table[j][i].next=-1;	
		}	
	}	
	while (1){
		fscanf(top_fp, "%d %d %d", &node1, &node2, &cost);
		if (feof(top_fp))
			break;
		input_Table(node1,node2,cost);
	
	}
	// i -> i cost 0
	Routing();
	print_table(output_fp);

	int cnt =0;
	while(1)
	{	fgets( message[cnt++], sizeof(message[cnt]), mes_fp );
		// printf( "%s", strTemp );
		if(feof(mes_fp)) break;
	}
	transMsg(output_fp,cnt-1);

	while (1){
		fscanf(cha_fp, "%d %d %d", &node1, &node2, &cost);
		if (feof(cha_fp))
			break;
		get_change(node1,node2,cost);
		Routing();
		print_table(output_fp);
		transMsg(output_fp,cnt-1);
	}
	
	
	fclose(top_fp);
	fclose(cha_fp);
	fclose(mes_fp );
	fclose(output_fp);
	return 0;
}

