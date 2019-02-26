#pragma once
#ifndef  TABU_H
#define TABU_H

#include<iostream>
#include<algorithm>
#include<vector>
using namespace std;

template<class T1,class T2>
using fd_pair = std::pair<T1, T2>;
class node {
public:
	int no;//与当前结点相邻的编号
	int dis;//距离
	node();
	node(int n, int d);
};
class Edge {
public:
	int f; //服务结点下标
	int t; //用户结点下标
	int dis; //距离
	Edge();
	Edge(int fc, int tc, int d);
};
class Solver {
	int v; //结点数量
	int e;//边数
	int f;//最长服务边--目前为止解决方案中最小的
	int mf;//当前解决方案的最长服务边
	int K;
	int p;//p个服务结点

	vector<bool> isService;
	vector<vector<int>> graph;//邻接矩阵
	vector<vector<node>> adj; //邻接表
	vector<int> S;//存放当前的解决方案
	vector<vector<int>> tabu_table;
	vector<int> user_tabu_table;
	vector<int> facility_tabu_table;
	vector<fd_pair<unsigned int,unsigned int>> F;
	vector<fd_pair<unsigned int, unsigned int>> D;
	vector<int> opt;
	vector<int> opt_m;
	vector<int> optS;//存放最优的解决方案
	int iter = 0;
public:
	Solver();
	Solver(int nodeN, int pN);
	void print_graph();
	bool cmp(node &n1, node &n2);
	bool initiate_graph(string fn);
	void sort_nodes();
	void create_initial_solution(); //构造初始解
	Edge find_max_edge();
	vector<int> get_k_neibors(int uno, int mdis);
	Edge find_move(int uno);
	void make_move(Edge &edge);
	int tabu_search(int opv);
	int Add_Facility(int vno);
	void Remove_Facility(int vno);
	void Find_Next(int v);
};

#endif