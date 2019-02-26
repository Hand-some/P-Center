#include"tabu.h"
#include<map>
#include<fstream>
#include<string>
/*node构造函数*/
node::node() {

}
node::node(int n, int d) {
	this->no = n;
	this->dis = d;
}
/*Edge构造函数*/
Edge::Edge() {

}
Edge::Edge(int fc, int tc, int d) {
	this->f = fc;
	this->t = tc;
	this->dis = d;
}
/*Solver构造函数*/
Solver::Solver() {
	opt = { 127,98,93,74,48,84,64,55,37,20,59,51,36,26,18,47,39,28,18,13,40,38,22,
			  15,11,38,32,18,13,9,30,29,15,11,30,27,15,29,23,13 }; //小算例最优解
	opt_m = { 10,20,30,40,50,60,70,80,90,100,110,120,130,140,150 };//大算例最优解
}
Solver::Solver(int nodeN, int pN) {
	this->v = nodeN;
	mf = 0;
	f = INT_MAX;
	p = pN;
	this->adj = vector<vector<node>>(nodeN, vector<node>(nodeN));
	this->graph = this->tabu_table = vector<vector<int>>(nodeN, vector<int>(nodeN, 0));
	user_tabu_table = vector<int>(nodeN, 0);
	facility_tabu_table = vector<int>(nodeN, 0);
	F = D = vector<fd_pair<unsigned int,unsigned int>>(nodeN);
	this->isService = vector<bool>(nodeN, false);
	opt = { 127,98,93,74,48,84,64,55,37,20,59,51,36,26,18,47,39,28,18,13,40,38,22,
			  15,11,38,32,18,13,9,30,29,15,11,30,27,15,29,23,13 };
	opt_m = { 10,20,30,40,50,60,70,80,90,100,110,120,130,140,150 };
}
void Solver::print_graph()
{
	cout << v <<"  "<< e <<" "<< p;
	/*for (int i = 0; i < v;++i) {
		for (int j = 0; j < v;++j) {
			cout << graph[i][j] << "   ";
		}
		cout << endl;
	}*/
}
bool Solver::cmp(node &n1, node &n2) {
	return n1.dis <= n2.dis;
}

/*核心算法实现*/
void Solver::create_initial_solution() {
	mf = 0;
	f = INT_MAX;
	vector<int> candidates;
	int count = 1;
	int userno = 0;//保存最长边的用户结点
//	srand(clock());
	int vno = rand() % v; //先随机选择一个结点，编号为vno
	S.push_back(vno);
	isService[vno] = true; //该结点作为服务结点
	while (S.size() != p) {
		/*找到与vno相邻的最长服务边，并且相邻结点不是服务结点*/
		count = adj[vno].size() - 1;
		while (count >= 0 && isService[adj[vno][count].no]) {
			count--;
		}
		count >= 0 ? userno = adj[vno][count].no : userno = 1;
		for (count = 0; count < adj[userno].size(); ++count) {
			int tno = adj[userno][count].no;
			//if (tno == vno)  break;
			if (!isService[tno]) candidates.push_back(tno);
		}
		//	srand(clock());
		vno = candidates[rand() % candidates.size()];
		S.push_back(vno);
		isService[vno] = true;
	}
	/*构造F&D表*/
	for (int i = 0; i < F.size(); ++i) {
		int min_i = 0;//最近服务结点下标
		int smin_i = 1;//次近服务结点下标 
		graph[i][S[min_i]] > graph[i][S[smin_i]] ? min_i = 1, smin_i = 0 : min_i = 0;
		for (int j = 2; j < S.size(); j++) {
			if (graph[i][S[j]] < graph[i][S[min_i]]) {
				smin_i = min_i;
				min_i = j;
			}
			else if (graph[i][S[j]] < graph[i][S[smin_i]]) {
				smin_i = j;
			}
		}
		F[i].first = S[min_i];
		D[i].first = graph[i][S[min_i]];
		F[i].second = S[smin_i];
		D[i].second = graph[i][S[smin_i]];
		mf = (mf < D[i].first ? D[i].first : mf); //mf=当前最长服务边
	}
	f = mf;
	optS = S;
}

Edge Solver::find_max_edge() {
	vector<Edge> longest_service_edge;
	/*找到最长服务边*/
	for (int i = 0; i < D.size(); ++i) {
		Edge tmpe = Edge();
		if (D[i].first == mf) {
			tmpe.f = F[i].first;//服务结点
			tmpe.t = i;//用户结点
			tmpe.dis = mf;
			longest_service_edge.push_back(tmpe);
		}
	}

	int index = rand() % longest_service_edge.size();
	return longest_service_edge[index];
}




vector<int> Solver::get_k_neibors(int uno, int mdis) {
	vector<int> ans;
	for (const auto &node_ : adj[uno]) {
		if (node_.dis >= mdis) break;
		else if (!isService[node_.no]) {
			ans.push_back(node_.no);
		}
	}
	return ans;
}


Edge Solver::find_move(int uno) {
	int best = INT_MAX;
	int tabu_best = INT_MAX;
	vector<Edge> spair;// 非禁忌的的最佳结点对
	vector<Edge> tabu_spair;//禁忌的最佳结点对 
	vector<int> less_node = this->get_k_neibors(uno, D[uno].first); /*根据最长服务边的用户结点uno，找到adj[uno]中比当前服务边mf小的用户结点集合*/
	Edge midRes = Edge(); //保存中间结点对
	int SC = 0;//保存增加一个结点后当前解决方案中的最长服务边
			   /*先备份F&D表*/
	vector<fd_pair<unsigned int,unsigned int>> FC(F);
	vector<fd_pair<unsigned int,unsigned int>> DC(D);
	for (const auto addNode : less_node) { //less_node集合里的每一个结点，都要试着加入一次
		SC = this->Add_Facility(addNode); //将addNode变成服务结点,SC为加入结点后的最长服务边
		map<int, int> Mf;  //Mf[i] = 删除当前服务结点i所新获得的最长服务边

		for (const auto s : S) {
			Mf[s] = 0;
		}

		for (int j = 0; j < v; ++j) {
			if (F[j].first != addNode && Mf[F[j].first] < D[j].second) {
				Mf[F[j].first] = D[j].second;
			}
		}


		for (const auto delNode : S) {
			/*对于每一个服务结点，不包括新加入的，找到<less_node[i],S[k]> 的最长服务边*/
			int tmp = max(SC, Mf[delNode]);
			midRes.f = delNode;//服务结点-用户结点
			midRes.t = addNode; //用户结点-服务结点
			if (iter >= user_tabu_table[delNode] || iter >= facility_tabu_table[addNode]) {  //非禁忌的
				if (best == tmp) {
					midRes.dis = best;
					spair.push_back(midRes);
				}
				else if (best > tmp) {
					best = tmp;
					spair.clear();
					midRes.dis = best;
					spair.push_back(midRes);
				}
			}
			else {//禁忌的
				if (tabu_best == tmp) {
					midRes.dis = tabu_best;
					tabu_spair.push_back(midRes);
				}
				else if (tabu_best > tmp) {
					tabu_best = tmp;
					midRes.dis = tabu_best;
					tabu_spair.clear();
					tabu_spair.push_back(midRes);
				}
			}
		}
		F = FC;
		D = DC;
		isService[addNode] = false;
	}

	/*随机选择出最优交换对*/
	if (tabu_spair.size() > 0 && tabu_best < best && tabu_best < f || spair.size() == 0) {
		//从禁忌对中选出一对交换
		int index = rand() % tabu_spair.size();
		midRes = tabu_spair[index];
	}
	else {
		//从非禁忌中选择一对交换
		int index = rand() % spair.size();
		midRes = spair[index];
	}
	return midRes;
}

void Solver::make_move(Edge &edge) {
	//增加结点
	this->Add_Facility(edge.t);
	S.push_back(edge.t);
	this->Remove_Facility(edge.f);
	//tabu_vector[edge.t][edge.f] = 100000;
	user_tabu_table[edge.t] = iter + p / 10 + rand() % 10;
	facility_tabu_table[edge.f] = iter + (v - p) / 10 + rand() % 100;

}
int Solver::tabu_search(int opv) {
	Edge ledge;//保存要交换的节点对
	while (f != opv || iter < 10000) {
		//cout << mf << " " << f << endl;
		ledge = this->find_max_edge();//找到最长服务边,用户结点为 ledge.t
		ledge = find_move(ledge.t);
		make_move(ledge);
		iter++;
		//更新                                  
		if (mf <= f) {
			f = mf;
			optS = S;
		}
	}
	return f;
}
int Solver::Add_Facility(int vno) {
	mf = 0;//保存增加完结点后的最长服务边
		   //	S.push_back(vno);
	isService[vno] = true;
	//更新F/D表
	for (int i = 0; i < this->v; ++i) {
		if (graph[i][vno] <= D[i].first) {
			F[i].second = F[i].first; D[i].second = D[i].first;
			F[i].first = vno; D[i].first = graph[i][vno];
		}
		else if (graph[i][vno] <= D[i].second) {
			F[i].second = vno; D[i].second = graph[i][vno];

		}
		if (mf < D[i].first) mf = D[i].first;
	}
	return mf;
}

void Solver::Remove_Facility(int vno) {
	mf = 0;
	S.erase(find(S.begin(), S.end(), vno));
	isService[vno] = false;
	/*更新F/D表*/
	for (int i = 0; i < this->v; ++i) {
		if (F[i].first == vno) {
			F[i].first = F[i].second;
			D[i].first = D[i].second;
			this->Find_Next(i);
		}
		else if (F[i].second == vno) {
			this->Find_Next(i);
		}
		if (mf < D[i].first) mf = D[i].first;
	}
}

void Solver::Find_Next(int v) {
	vector<fd_pair<unsigned int,unsigned int>> ans; //ans[0] = Dv1 ans[1] = Fv1
	fd_pair<unsigned int,unsigned int> tmp;
	int currdis = INT_MAX;
	for (const auto sno : S) {
		if (sno != F[v].first) {
			if (graph[v][sno] < currdis) {
				tmp.first = sno;
				currdis = tmp.second = graph[v][sno];
				ans.clear();
				ans.push_back(tmp);
			}
			else if (graph[v][sno] == currdis) {
				tmp.first = sno;
				tmp.second = currdis;
				ans.push_back(tmp);
			}
		}

	}
	int index = rand() % ans.size();
	F[v].second = ans[index].first;
	D[v].second = ans[index].second;
}

/*用文件构造图，参数为文件名*/
bool Solver::initiate_graph(string fn) {
	ifstream infile(fn, ios::in);
	if (!infile) { /*文件打开失败*/
		cout << "file open error!" << endl;
		exit(1);
	}
	infile >> v >> e >> p;
	graph = vector<vector<int>>(v, vector<int>(v, INT_MAX));
	tabu_table = vector<vector<int>>(v, vector<int>(v, 0)); //单禁忌
	facility_tabu_table = user_tabu_table = vector<int>(v, 0);//双禁忌
	adj = vector<vector<node>>(v);
	F = D = vector<fd_pair<unsigned int,unsigned int>>(v);
	isService = vector<bool>(v, false);

	int snode = 0, enode = 0, dis = 0;
	while (infile >> snode >> enode >> dis) {//逐行读取文件
		//if(graph[snode][enode] > dis)
		snode -= 1;
		enode -= 1;
		graph[snode][enode] = graph[enode][snode] = dis;
		graph[snode][snode] = graph[enode][enode] = 0;
	}
	infile.close();
	/*弗洛伊德算法补全边*/
	for (int i = 0; i < v ; ++i) {
		for (int j = 0; j < v; ++j) {
			for (int k = 0; k < v; ++k) {
				if (graph[i][j] == INT_MAX || graph[k][j] == INT_MAX) continue;
				if (graph[i][k] > graph[i][j] + graph[j][k]) {
					graph[i][k] = graph[k][i] = graph[i][j] + graph[j][k];
				}
			}
		}
	}

	for (int i = 0; i < v; ++i) {
		for (int j = 0; j < v; ++j) {
			node n1 = node(j, graph[i][j]);
			adj[i].push_back(n1);
		}
	}
	this->sort_nodes();//给邻居结点排序
}

void Solver::sort_nodes()
{
	for (int i = 0; i < v; ++i) {
		sort(adj[i].begin(), adj[i].end(), [](node &n1, node &n2) { return n1.dis < n2.dis; });
	}

}
