#include"tabu.h"
#include<ctime>
#include<string>
#include<cstdlib>

int main() 
{
	clock_t st, et;
	srand(clock());
	vector<int> opt = { 127,98,93,74,48,84,64,55,37,20,59,51,36,26,18,47,39,28,18,13,40,38,22,
			  15,11,38,32,18,13,9,30,29,15,11,30,27,15,29,23,13 };
	for (int count_ = 1; count_ <= opt.size(); ++count_) { //批量调试
		st = clock();
		Solver sol = Solver();
		string fn = "./instance/pmed";
		fn += to_string(count_);
		fn += ".txt";
		cout << "fn= " << fn << endl;
		sol.initiate_graph(fn);
		//sol.print_graph();
		sol.create_initial_solution();
		//禁忌搜索获得最优解
		int f = sol.tabu_search(opt[count_-1]);
		et = clock();
		cout << "pmed" << count_ << ".txt 的最优值=" << f << "    运行时间： " << (double)(et - st) / CLOCKS_PER_SEC << "S" << endl;
	}
	system("pause");
	return 0;
}