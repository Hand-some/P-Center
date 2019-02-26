#include"tabu.h"
#include<map>
#include<fstream>
#include<string>
/*node���캯��*/
node::node() {

}
node::node(int n, int d) {
	this->no = n;
	this->dis = d;
}
/*Edge���캯��*/
Edge::Edge() {

}
Edge::Edge(int fc, int tc, int d) {
	this->f = fc;
	this->t = tc;
	this->dis = d;
}
/*Solver���캯��*/
Solver::Solver() {
	opt = { 127,98,93,74,48,84,64,55,37,20,59,51,36,26,18,47,39,28,18,13,40,38,22,
			  15,11,38,32,18,13,9,30,29,15,11,30,27,15,29,23,13 }; //С�������Ž�
	opt_m = { 10,20,30,40,50,60,70,80,90,100,110,120,130,140,150 };//���������Ž�
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

/*�����㷨ʵ��*/
void Solver::create_initial_solution() {
	mf = 0;
	f = INT_MAX;
	vector<int> candidates;
	int count = 1;
	int userno = 0;//������ߵ��û����
//	srand(clock());
	int vno = rand() % v; //�����ѡ��һ����㣬���Ϊvno
	S.push_back(vno);
	isService[vno] = true; //�ý����Ϊ������
	while (S.size() != p) {
		/*�ҵ���vno���ڵ������ߣ��������ڽ�㲻�Ƿ�����*/
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
	/*����F&D��*/
	for (int i = 0; i < F.size(); ++i) {
		int min_i = 0;//����������±�
		int smin_i = 1;//�ν��������±� 
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
		mf = (mf < D[i].first ? D[i].first : mf); //mf=��ǰ������
	}
	f = mf;
	optS = S;
}

Edge Solver::find_max_edge() {
	vector<Edge> longest_service_edge;
	/*�ҵ�������*/
	for (int i = 0; i < D.size(); ++i) {
		Edge tmpe = Edge();
		if (D[i].first == mf) {
			tmpe.f = F[i].first;//������
			tmpe.t = i;//�û����
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
	vector<Edge> spair;// �ǽ��ɵĵ���ѽ���
	vector<Edge> tabu_spair;//���ɵ���ѽ��� 
	vector<int> less_node = this->get_k_neibors(uno, D[uno].first); /*���������ߵ��û����uno���ҵ�adj[uno]�бȵ�ǰ�����mfС���û���㼯��*/
	Edge midRes = Edge(); //�����м����
	int SC = 0;//��������һ������ǰ��������е�������
			   /*�ȱ���F&D��*/
	vector<fd_pair<unsigned int,unsigned int>> FC(F);
	vector<fd_pair<unsigned int,unsigned int>> DC(D);
	for (const auto addNode : less_node) { //less_node�������ÿһ����㣬��Ҫ���ż���һ��
		SC = this->Add_Facility(addNode); //��addNode��ɷ�����,SCΪ��������������
		map<int, int> Mf;  //Mf[i] = ɾ����ǰ������i���»�õ�������

		for (const auto s : S) {
			Mf[s] = 0;
		}

		for (int j = 0; j < v; ++j) {
			if (F[j].first != addNode && Mf[F[j].first] < D[j].second) {
				Mf[F[j].first] = D[j].second;
			}
		}


		for (const auto delNode : S) {
			/*����ÿһ�������㣬�������¼���ģ��ҵ�<less_node[i],S[k]> ��������*/
			int tmp = max(SC, Mf[delNode]);
			midRes.f = delNode;//������-�û����
			midRes.t = addNode; //�û����-������
			if (iter >= user_tabu_table[delNode] || iter >= facility_tabu_table[addNode]) {  //�ǽ��ɵ�
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
			else {//���ɵ�
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

	/*���ѡ������Ž�����*/
	if (tabu_spair.size() > 0 && tabu_best < best && tabu_best < f || spair.size() == 0) {
		//�ӽ��ɶ���ѡ��һ�Խ���
		int index = rand() % tabu_spair.size();
		midRes = tabu_spair[index];
	}
	else {
		//�ӷǽ�����ѡ��һ�Խ���
		int index = rand() % spair.size();
		midRes = spair[index];
	}
	return midRes;
}

void Solver::make_move(Edge &edge) {
	//���ӽ��
	this->Add_Facility(edge.t);
	S.push_back(edge.t);
	this->Remove_Facility(edge.f);
	//tabu_vector[edge.t][edge.f] = 100000;
	user_tabu_table[edge.t] = iter + p / 10 + rand() % 10;
	facility_tabu_table[edge.f] = iter + (v - p) / 10 + rand() % 100;

}
int Solver::tabu_search(int opv) {
	Edge ledge;//����Ҫ�����Ľڵ��
	while (f != opv || iter < 10000) {
		//cout << mf << " " << f << endl;
		ledge = this->find_max_edge();//�ҵ�������,�û����Ϊ ledge.t
		ledge = find_move(ledge.t);
		make_move(ledge);
		iter++;
		//����                                  
		if (mf <= f) {
			f = mf;
			optS = S;
		}
	}
	return f;
}
int Solver::Add_Facility(int vno) {
	mf = 0;//��������������������
		   //	S.push_back(vno);
	isService[vno] = true;
	//����F/D��
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
	/*����F/D��*/
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

/*���ļ�����ͼ������Ϊ�ļ���*/
bool Solver::initiate_graph(string fn) {
	ifstream infile(fn, ios::in);
	if (!infile) { /*�ļ���ʧ��*/
		cout << "file open error!" << endl;
		exit(1);
	}
	infile >> v >> e >> p;
	graph = vector<vector<int>>(v, vector<int>(v, INT_MAX));
	tabu_table = vector<vector<int>>(v, vector<int>(v, 0)); //������
	facility_tabu_table = user_tabu_table = vector<int>(v, 0);//˫����
	adj = vector<vector<node>>(v);
	F = D = vector<fd_pair<unsigned int,unsigned int>>(v);
	isService = vector<bool>(v, false);

	int snode = 0, enode = 0, dis = 0;
	while (infile >> snode >> enode >> dis) {//���ж�ȡ�ļ�
		//if(graph[snode][enode] > dis)
		snode -= 1;
		enode -= 1;
		graph[snode][enode] = graph[enode][snode] = dis;
		graph[snode][snode] = graph[enode][enode] = 0;
	}
	infile.close();
	/*���������㷨��ȫ��*/
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
	this->sort_nodes();//���ھӽ������
}

void Solver::sort_nodes()
{
	for (int i = 0; i < v; ++i) {
		sort(adj[i].begin(), adj[i].end(), [](node &n1, node &n2) { return n1.dis < n2.dis; });
	}

}
