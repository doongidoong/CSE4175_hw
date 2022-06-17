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


// void print(int a){
// 	for(int i=0;i<node_num;i++){
// 		printf(" %d %d : next= %d, cost= %d \n",a,i,rt_table[a][i].next ,rt_table[a][i].cost);
// 	}
// 	printf("\n");
// }


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

int SPT[100][100] = {-1};

void dijkstra(int start){
	int P[100];
	int visited[100];
	memset(visited, 0, sizeof(visited));
	memset(SPT[start], -1, sizeof(SPT[start]));
	memset(P, -1, sizeof(P));

	for (int i = 0; i < node_num; i++){
		if (rt_table[start][i].direct < INF && start != i){
			P[i] = start;
			rt_table[start][i].next = i;
		}
	}

	SPT[start][0] = start;
	P[start] = -1;
	visited[start] = 1;
	for (int i = 1; i < node_num; i++){
		int min = INF;
		int idx = -1;
		for (int j = 0; j < node_num; j++){
			if (P[j] != -1 && visited[j] == 0){
				if (rt_table[start][j].cost < min){
					min = rt_table[start][j].cost;
					idx = j;
				}
			}
		}
		if (idx ==-1)break;
		SPT[start][i] = idx;
		visited[idx] = 1;
		rt_table[start][idx].cost = min;
		int j = idx;
		while (P[j] != start) j = P[j];
		rt_table[start][idx].next = j;
		for (int j = 0; j < node_num; j++){
			if (visited[j] == 0){
				if (rt_table[start][j].cost > rt_table[idx][j].direct + rt_table[start][idx].cost){
					rt_table[start][j].cost = rt_table[idx][j].direct + rt_table[start][idx].cost;
					P[j] = idx;
				}
				else if (rt_table[start][j].cost  == rt_table[idx][j].direct + rt_table[start][idx].cost){
					if (idx < P[j]) P[j] = idx;
				}
			}
		}
	}


}
void Routing(){
	int i;
	for(i=0;i<node_num;i++){
		dijkstra(i);
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
		rt_table[a][b].direct = INF;
		rt_table[b][a].direct = INF;
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
		exit(1);
	}
	if ((top_fp == NULL)||(mes_fp == NULL)||(cha_fp == NULL)){
		fprintf(stderr, "Error: open input file.\n");
		exit(1);
	}
	output_fp = fopen("output_ls.txt", "w"); //output
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
			rt_table[i][j].direct=INF;
			rt_table[j][i].direct=INF;
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
	Routing();
	print_table(output_fp);

	int cnt =0;
	while(1)
	{	fgets( message[cnt++], sizeof(message[cnt]), mes_fp );
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
	fclose( mes_fp );
	fclose(output_fp);
	return 0;
}

