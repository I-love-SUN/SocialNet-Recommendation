#include<iostream>
#include<algorithm>
#include<stack>
#include<fstream>
#include<sstream>
#include<vector>
#include<string>
#include<stdlib.h>
#include<math.h>
#include"中文字符转换.cpp"
#define MAX 4100
#define pi 3.1415926535897932384626433832795
#define EARTH_RADIUS 6378.137 //地球半径 KM
using namespace std;
int personLink[MAX][MAX];  //人际交往距离 
float hobby_simi[MAX][MAX];  //爱好相似度 
float location_simi[MAX][MAX];  //同城同省概率 
int path[MAX][MAX];
string hobbies[16]={"篮球","足球","游泳","乒乓球","网球","骑行","跑步","跳舞",
					"爬山","滑板","绘画","美食","美妆","追剧","阅读","编程"};
struct relation{
	int nodeid; //对方的id 
	float intimacy; //亲密度
	relation(){
		nodeid=-1;
		intimacy=0;
	} 
};
struct loca{
	string cityname;
	float longitude;
	float latitude;
};
relation relations[MAX][MAX]; //亲密度 
struct person_node {
	int node_id; //结点有个编号
	int age;
	string name;
	int hobbys[16];  //爱好，可以基于爱好推荐，爱好是01向量，规定了没人最多有6种爱好
	loca location;  //地理位置，可以匹配同城
	string school;    //校友选项
	int friends;  //有直接关系的数目 
};

struct Graph {
	int n;//person节点数
	person_node net[MAX];
	void setup(); //初始化，读入图 
	void intimacy_caculate(int i);  //亲密度计算
	void all_intimacy();  
	bool have_relation(int i, int j); //两人是否有关系，及是否直接认识 
	void ShortestPath_Floyd(int n);//计算最短路径长度
	int get_distance(int id1,int id2);//两人的社交距离 
	float get_hobby_similarity(int i,int j); //两人的爱好相似度 
	void all_hobby_similarity(); //整体的爱好相似度，存在矩阵里
	float location_match(wstring,wstring);//地名匹配度，判断是否来自一个地方 
	float location_match(loca l1,loca l2);//按照地理位置匹配 
	void all_location_match(); //整体的同城率 
	float distance_trans(int dist);//交往距离转化函数 
	void recommendation(int i); //推荐第i个人的潜在好友 
	
}; 

int Graph::get_distance(int id1,int id2){
	return personLink[id1][id2];
}

bool Graph::have_relation(int a, int b) {
	return personLink[a][b]==1;
}

void Graph::setup() {
	string line;
	ifstream readout(".\\file.csv",ios::in);
	if (!readout.is_open()) {
		cout << "Failed to open the file.\n";
	}
	while (getline(readout,line))//getline(inFile, line)表示按行读取CSV文件中的数据  
	{
		string field;
		istringstream sin(line); //将整行字符串line读入到字符串流sin中
		getline(sin, field, ','); //将字符串流sin中的字符读入到field字符串中，以逗号为分隔符 
		int i = atoi(field.c_str());
		getline(sin, field, ','); //将字符串流sin中的字符读入到field字符串中，以逗号为分隔符 
		int j = atoi(field.c_str());
		if(i!=j){
    		personLink[i][j] = 1;
	    	personLink[j][i] = 1;
	    }
	    net[i].friends++;
		if (n < i) { //记录多少人
			n = i;
		}
	}
	readout.close();
	
	ifstream readhobby(".\\hobbys.txt", ios::in);
	if (!readhobby.is_open()) {
		cout << "Faild to open the file!\n";
	}
	int tmp;
	for(int i=0;i<n;i++){
		net[i].node_id=i;
	}
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < 16; j++) {
			readhobby >> tmp;
			net[i].hobbys[j] = tmp;
		}
	}
	readhobby.close();
	
	ifstream readaddress(".\\locations.txt");
	if(!readaddress.is_open()){
		cout<<"Failed to open the file!\n";
	}
	int i=0;
	for(i=0;i<n;i++){
		readaddress>>net[i].location.cityname>>net[i].location.latitude>>net[i].location.longitude;
	}
	readaddress.close(); 
//	for(i=0;i<n;i++){
//		cout<<net[i].location.cityname<<" "
//			<<net[i].location.latitude<<" "
//			<<net[i].location.longitude<<endl;
//	}

	ifstream readname(".\\nameinfo.txt");
	if(!readname.is_open()){
		cout<<"Failed to open the file!";
	}
	for(i=0;i<n;i++){
		getline(readname,line);
		net[i].name=line;
	}
	cout<<"数据读取完毕！"<<endl;
	readname.close();
	//ShortestPath_Floyd(n);
	
	
	for(i=0;i<n;i++){
		for(int j=0;j<n;j++){
			relations[i][j].nodeid=j;
			relations[i][j].intimacy=0;
		}
	}
	cout<<"亲密度矩阵初始化完毕！\n";
}

void Graph::ShortestPath_Floyd(int n){
	for(int i = 0;i<n;i++){
		for(int j = 0;j<n;j++){
			if(i!=j){
				path[i][j]=i;
			}
			else if(i==j){
				path[i][j]=-1;
			}
			if(personLink[i][j] == 0&&i!=j){
				personLink[i][j] = 100000;
				path[i][j]=i;
			}
			
		}
	}
	for(int k = 0; k<n ; k++){
		for(int v = 0;v<n;v++){
			for(int w = 0; w<n ; w++){
				if(personLink[v][w] > personLink[v][k] + personLink[k][w]){
					personLink[v][w] = personLink[v][k] + personLink[k][w];
					path[v][w] = path[k][w];
				}
//				else if(personLink[v][w] == personLink[v][k] + personLink[k][w]&&hobby_simi[v][w]>hobby_simi[k][w]){
//					path[v][w]=path[k][w];
//				}
			}
		}
	}
	cout<<"最近社交距离计算完毕！"<<endl;
//	for(int i = 0;i<n;i++){
//		for(int j = 0;j<n;j++){
//			printf("%d  ",personLink[i][j]);
//		}
//		printf("\n");
//	}
}


float Graph::get_hobby_similarity(int per1,int per2){ //计算爱好相似度，生成爱好相似度矩阵 
	float common=0,total=0;
	int i;
	for(i=0;i<16;i++){
		if(net[per1].hobbys[i]==1||net[per2].hobbys[i]==1){
			total++;
		}
		if(net[per1].hobbys[i]==1&&net[per2].hobbys[i]==1){
			common++; 
		}
	}
	hobby_simi[per1][per2]=hobby_simi[per2][per1]=(float)common/total;
	return (float)common/total;
}

void Graph::all_hobby_similarity(){
	int i,j;
	for(i=0;i<n;i++){
		for(j=0;j<n;j++){
			if(i==j) continue;
			get_hobby_similarity(i,j);
		}
	}
//	for(i=0;i<n;i++){
//		for(j=0;j<n;j++){
//			printf("%.3f ",hobby_simi[i][j]);
//		}
//		printf("\n");
//	}
	cout<<"爱好相似度计算完毕！"<<endl;
} 

int c[60][60];
float Graph::location_match(wstring s1,wstring s2){ //动态规划求最长子序列长度，不会有同名城市，所以以此计算匹配度 
    int m=wcslen(s1.c_str())+1;
    int n=wcslen(s2.c_str())+1;
    for(int i=0;i<m;i++)
        c[i][0]=0;
    for(int i=0;i<n;i++)
        c[0][i]=0;
    for(int i=0;i<m-1;i++){
        for(int j=0;j<n-1;j++){
            if(s1[i]==s2[j]){
                c[i+1][j+1]=c[i][j]+1;
            }
            else if(c[i][j+1]>=c[i+1][j]){
                c[i+1][j+1]=c[i][j+1];
            }
            else{
                c[i+1][j+1]=c[i+1][j];
            }
        }
    }
    int res=c[m-1][n-1];
    if(res<4) return 0;
    else return (float)res/(m+n-2-res);
}

float rad(float d){
    return d * pi /180.0;
}

float Graph::location_match(loca l1,loca l2){
	float a;
   	float b;
   	float radLat1 = rad(l1.latitude);
    float radLat2 = rad(l2.latitude);
    a = radLat1 - radLat2;
    b = rad(l1.longitude) - rad(l2.longitude);
    float s = 2 * asin(sqrt(pow(sin(a/2),2) + cos(radLat1)*cos(radLat2)*pow(sin(b/2),2)));
    s = s * EARTH_RADIUS;
    if(s==0) return 1;
    float res=1-atan(s/1000)/pi*2;
    return res;
    //return atan(s);
}

void Graph::all_location_match(){
	int i,j;
	for(i=0;i<n;i++){
		for(j=0;j<n;j++){
			if(i==j) continue;
//			wstring location1=str2wstr(net[i].location);
//			wstring location2=str2wstr(net[j].location);
			location_simi[i][j]=location_match(net[i].location,net[j].location);
			location_simi[j][i]=location_simi[i][j];
		}
	}
	cout<<"城市匹配计算完毕！"<<endl;
//	for(i=0;i<n;i++){
//		for(j=0;j<n;j++){
//			printf("%.2f  ",location_simi[i][j]);
//		}
//		cout<<endl;
//	}
}

void array_generate(int n){
	int i,j;
	ofstream write;
	write.open(".\\p1.csv",ios::out);
	i=211;
	for(j=0;j<n;j++){
		write<<personLink[i][j]<<',';
		write<<hobby_simi[i][j]<<',';
		write<<location_simi[i][j];
		write<<endl;
	}
//	for(j=0;j<n;j++){
//		write<<hobby_simi[i][j]<<" ";
//	}
//	cout<<endl;
//	for(j=0;j<n;j++){
//		write<<location_simi[i][j]<<" ";
//	}
	write.close();
	cout<<"向量生成完毕！"<<endl;
} 

float Graph::distance_trans(int dist){
	float trans=exp(1-dist);
	if(dist==1) return 1.00;
	else return 1.5*trans;
} 

void Graph::intimacy_caculate(int i) {
	float hobby=0.45,dist=0.10,loca=0.45;
	int j,select;
	printf("请选择推荐偏好：\n");
	printf("1.同城优先  2.爱好相符  3.关系推荐\n");
	scanf("%d",&select);
	if(select==1){
		dist=0.15;
		loca=0.65;
		hobby=0.20;
	}
	else if(select==2){
		hobby=0.80;
		dist=0.10;
		loca=0.10;
	}
	else if(select==3){
		dist=0.60;
		loca=0.10;
		hobby=0.30;
	}
	else{
		printf("输入非法！");
		return ;
	}
	for(j=0;j<n;j++){
		double intimacy,nomination;
		double hobby_sec,dist_sec,location_sec;//爱好、社交距离、同城占比
		hobby_sec= hobby_simi[i][j];
		dist_sec=distance_trans(personLink[i][j]);
		location_sec=location_simi[i][j];
	//	nomination=sqrt(hobby_sec*hobby_sec+dist_sec*dist_sec+location_sec*location_sec);//归一化过程
		hobby_sec=hobby_sec*hobby;
		dist_sec=dist_sec*dist;
		location_sec=location_sec*loca; 
		intimacy=hobby_sec+dist_sec+location_sec;
		if(i==j){
			intimacy=-1;
		}
		relations[i][j].intimacy=intimacy;
	//	cout<<hobby_sec<<"+"<<dist_sec<<"+"<<location_sec<<" = "<<intimacy<<endl;
	}
//	for(j=0;j<n;j++){
//		cout<<relations[i][j].nodeid<<"--"<<relations[i][j].intimacy;
//		printf("%d--%.2f ",relations[i][j].nodeid,relations[i][j].intimacy);
//	}
	printf("\n");
}
void Graph::all_intimacy(){
	int i;
	for(i=0;i<n;i++){
		intimacy_caculate(i);
	}
	for(i=0;i<n;i++){
		for(int j=0;i<n;j++){
			printf("%d--%f ",relations[i][j].nodeid,relations[i][j].intimacy);
		}
		printf("\n");
	}
}
bool cmp(relation r1,relation r2){
	return r1.intimacy>r2.intimacy;
}
void Graph::recommendation(int i){
//	for(int j=0;j<n;j++){
//		printf("%.3f  ",location_simi[i][j]);
//	}
//	for(int j=0;j<n;j++){
//		printf("%d  ",path[i][j]);
//	}
//	printf("\n");
//	for(int j=0;j<n;j++){
//		printf("%d  ",personLink[i][j]);
//	}
//	for(int j=0;j<n;j++){
//		printf("%.3f  ",location_simi[i][j]);
//	}
//	printf("\n");
	
	sort(relations[i],relations[i]+n,cmp);//推荐排序1:亲密度排序
//	int i1,j;
//	relation tmp;
//	for(i1=0;i1<n;i1++){ //第二排序： 
//		j=i1-1;
//		tmp=relations[i][i1];//之后的操作a[i]被污染，需要有一个tmp保存 
//		while(j>=0&&tmp.intimacy==relations[i][j].intimacy&&net[tmp.nodeid].friends<net[relations[i][j].nodeid].friends){
//			relations[i][j+1]=relations[i][j];
//			j--;
//		}
//		relations[i][j+1]=tmp;
//	}
	
	string root=".\\recommend";
	string vertex=".\\path";
	string number=to_string(i);
	root=root+number;
	vertex=vertex+number;
	ofstream write,writev;
	string out1=root+".csv";
	string out2=vertex+".csv";
	write.open(out1,ios::out);
	writev.open(out2,ios::out);
	write<<"id"<<','<<"name"<<','<<"location"<<','<<"hobbby similarity"<<','
		 <<"location similatity"<<','<<"social distance"<<','<<endl; 
	write<<net[i].node_id<<','<<net[i].name<<','<<net[i].location.cityname<<',';
//	for(int a=0;a<16;a++){
//		if(net[i].hobbys[a]==1){
//			write<<hobbies[a]<<',';
//		}
//	}
	write<<endl; 
	int count=10;
	for(int j=0;j<n;j++){
		if(count<1) break; //推荐潜在亲密度最高的前10名 
		int per=relations[i][j].nodeid;
		if(personLink[i][per]>5||personLink[i][per]<2){
			continue;
		}
		count--;
		
		printf("\n\n");
		printf("推荐潜在好友%d：%d\n",10-count,per);
		
		
		stack<int> Path;
		int k=path[i][per];
		while(k!=i&&k!=-1){
			Path.push(k);
			k=path[i][k];
			//printf("%d  ",k);
		}
		Path.push(i);
		printf("关系路径：");
		//write<<"关系路径：";
		while(!Path.empty()){
			k=Path.top();
			Path.pop();
			printf("%d->",k);
			if(k!=i){
				write<<net[k].node_id<<','<<net[k].name<<','<<net[k].location.cityname<<','
				<<hobby_simi[i][k]<<','<<location_simi[i][k]<<','<<distance_trans(personLink[i][k])<<',';
//				for(int a=0;a<16;a++){
//					if(net[k].hobbys[a]==1){
//						write<<hobbies[a]<<',';
//					}
//				}
				write<<endl;
			}
			//write<<k<<" ";
			//writev<<k<<',';
			if(!Path.empty()){
				writev<<k<<','<<Path.top()<<endl;
			}
		}
		printf("%d \n",per);
		//write<<per<<endl;
		writev<<k<<','<<per<<endl;
		
		printf("您与ta的潜在亲密度为%.3f\n",relations[i][j].intimacy);
		printf("爱好相似度为：%.3f,社交距离权重为%.3f,同城系数为%.3f\n",hobby_simi[i][per],distance_trans(personLink[i][per]),location_simi[i][per]);
		cout<<i<<"来自"<<net[i].location.cityname<<","<<per<<"来自"<<net[per].location.cityname<<endl;
//		cout<<per<<"-"<<net[per].name<<"-"<<net[per].location<<endl;
		//write<<"关系数据："<<hobby_simi[i][per]<<" "<<distance_trans(personLink[i][per])<<" "<<location_simi[i][per]<<endl;
		
		printf("您与ta的爱好相似度为：%.3f\n",hobby_simi[i][per]);
		printf("在");
		for(int a=0;a<16;a++){
			if(net[i].hobbys[a]==1&&net[per].hobbys[a]==1){
				cout<<hobbies[a]<<" ";
			}
		}
		printf("等有共同点\n");
		
		write<<net[per].node_id<<','<<net[per].name<<','<<net[per].location.cityname<<','<<hobby_simi[i][per]<<','
			 <<location_simi[i][per]<<','<<distance_trans(personLink[i][per])<<',';
//		for(int a=0;a<16;a++){
//			if(net[per].hobbys[a]==1){
//				write<<hobbies[a]<<',';
//			}
//		}
		write<<endl; 
	}
	for(i=0;i<n;i++){
		for(int j=0;j<n;j++){
			relations[i][j].nodeid=j;
			relations[i][j].intimacy=0;
		}
	}
	write.close();
	writev.close();
}
