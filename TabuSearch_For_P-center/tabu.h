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
	int no;//�뵱ǰ������ڵı��
	int dis;//����
	node();
	node(int n, int d);
};
class Edge {
public:
	int f; //�������±�
	int t; //�û�����±�
	int dis; //����
	Edge();
	Edge(int fc, int tc, int d);
};
class Solver {
	int v; //�������
	int e;//����
	int f;//������--ĿǰΪֹ�����������С��
	int mf;//��ǰ���������������
	int K;
	int p;//p��������

	vector<bool> isService;
	vector<vector<int>> graph;//�ڽӾ���
	vector<vector<node>> adj; //�ڽӱ�
	vector<int> S;//��ŵ�ǰ�Ľ������
	vector<vector<int>> tabu_table;
	vector<int> user_tabu_table;
	vector<int> facility_tabu_table;
	vector<fd_pair<unsigned int,unsigned int>> F;
	vector<fd_pair<unsigned int, unsigned int>> D;
	vector<int> opt;
	vector<int> opt_m;
	vector<int> optS;//������ŵĽ������
	int iter = 0;
public:
	Solver();
	Solver(int nodeN, int pN);
	void print_graph();
	bool cmp(node &n1, node &n2);
	bool initiate_graph(string fn);
	void sort_nodes();
	void create_initial_solution(); //�����ʼ��
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