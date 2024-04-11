// #pragma GCC optimize(3)
#include <iostream>
#include <algorithm>
#include <queue>
#include <fstream>
#include <sstream>
#include <chrono>
using namespace std;
typedef pair<int, int> PII;

struct Con_Com {
	int bh, cnt;
} cc[25710000];

struct Region {
    int bh;
    long long cost;
    int si;
} re[12];
// int partition_element[11][33001];
vector<vector<int>> partition_component;
vector<vector<int>> partition_node_component;
vector<vector<int>> id_list;
int partition_number_cnt[9][282640000];
int partition_bool[9][282640000];
int partition_cnt = 8;

int new_property_num[] = {6, 4, 8, 3, 8, 3, 3, 4};

int judge_label(int node_id) {
	if (node_id >= 0 && node_id <= 220096051) {
		return 0;
	}
	if (node_id >= 220096052 && node_id <= 220097511) {
		return 1;
	}
	if (node_id >= 220097512 && node_id <= 220546137) {
		return 2;
	}
	if (node_id >= 220546138 && node_id <= 220546208) {
		return 3;
	}
	if (node_id >= 220546209 && node_id <= 278533231) {
		return 4;
	}
	if (node_id >= 278533232 && node_id <= 282613835) {
		return 5;
	}
	if (node_id >= 282613836 && node_id <= 282629915) {
		return 6;
	}
	if (node_id >= 282629916 && node_id <= 282637870) {
		return 7;
	}
	return 0;
}

bool cmp(Con_Com a, Con_Com b) {
	return a.cnt > b.cnt;
}

int cmp1(Region a, Region b) {
    if (a.si != b.si) {
        return a.si > b.si;
    }
    else{
        if (a.cost != b.cost) {
            return a.cost < b.cost;
        }
    }
    return a.bh < b.bh;
}

int cmp2(Region a, Region b) {
    return a.bh < b.bh;
}

void Stringsplit(string str, const char split,vector<string>& res)
{
	istringstream iss(str);
	string token;
	while (getline(iss, token, split))
	{
		res.push_back(token);
	}
}

int main() {
    auto start = std::chrono::high_resolution_clock::now();

	ifstream fin("/sys/fs/cgroup/new_SF100/connected_size_all_v4.txt");
	int conn_cnt = 0, bh, num, tcnt;
	// fin >> conn_cnt;
    
	int k = 0;
    long long total_cost = 0;
	while (fin >> bh >> num) {
		cc[k].bh = bh;
		cc[k].cnt = num;
        total_cost += num;
		k++;
        conn_cnt++;
	}
    long long threshold = total_cost / partition_cnt;
	sort(cc, cc + conn_cnt, cmp);

    ifstream fin1("/sys/fs/cgroup/new_SF100/region_component_v4.txt");
	string line;
	int pcnt = 0;
    vector<int> tini;
    tini.push_back(0);
    id_list.push_back(tini);
	while (getline(fin1, line)) {
		vector<int> l_id;
		int pos = line.find(":");
		string tline = line.substr(pos + 1);
        	pcnt++;
        	int now_loop = 0;

        	vector<string> strList;
        	Stringsplit(tline, ',', strList);
        	for (int i = 0; i < strList.size(); i++) {
            		int tn = stoi(strList[i]);
            		l_id.push_back(tn);
        	}
		id_list.push_back(l_id);
	}

    for (int i = 0; i < partition_cnt; i++) {
        re[i].bh = i;
        re[i].cost = 0;
        re[i].si = 0;
    }
    string origin_str = "";
    for (int i = 0; i < partition_cnt; i++) {
        for (int j = 0; j < 282640000; j++) {
            partition_number_cnt[i][j] = 0;
            partition_bool[i][j] = 0;
        }
    }

    ofstream fout, fout1, fout2;

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> duration = end - start;

    start = std::chrono::high_resolution_clock::now();
	tcnt = conn_cnt;
    vector<int> t_node_ini;
    t_node_ini.push_back(-1);
    for (int i = 0; i < partition_cnt; i++) {
        partition_node_component.push_back(t_node_ini);
    }
    for (int i = 0; i < partition_cnt; i++) {
        for (int k = 0; k < id_list[cc[i].bh].size(); k++){
            int node_id = id_list[cc[i].bh][k];
            partition_node_component[i].push_back(node_id);
            partition_bool[i][node_id] = 1;
            partition_number_cnt[i][node_id]++;
        }
        re[i].cost += cc[i].cnt;
        vector<int> tb;
        tb.push_back(cc[i].bh);
        partition_component.push_back(tb);
    }

    for (int i = partition_cnt; i < conn_cnt; i++) {
        for (int j = 0; j < partition_cnt; j++) {
            int tsum = 0;
            for (int k = 0; k < id_list[cc[i].bh].size(); k++){
                int node_id = id_list[cc[i].bh][k];
                int tn = partition_number_cnt[j][node_id];
                tsum += tn;
            }
            re[j].si = tsum;
        }

        sort(re, re + partition_cnt, cmp1);
        for (int j = 0; j < partition_cnt; j++) {
            if (re[j].cost + cc[i].cnt < threshold) {
                for (int k = 0; k < id_list[cc[i].bh].size(); k++){
                    int node_id = id_list[cc[i].bh][k];
                    if (partition_bool[re[j].bh][node_id] == 0) {
                        partition_node_component[re[j].bh].push_back(node_id);
                        partition_bool[re[j].bh][node_id] = 1;
                    }
                    else {
                        int temp = new_property_num[judge_label(node_id)];
                        re[j].cost -= temp;
                        total_cost -= temp;
                        threshold = total_cost / partition_cnt;
                    }
                    partition_number_cnt[j][node_id]++;
                }
                re[j].cost += cc[i].cnt;
                partition_component[re[j].bh].push_back(cc[i].bh);
                break;
            }
            else {
                continue;
            }
        }
    }

    end = std::chrono::high_resolution_clock::now();

    duration = end - start;

    start = std::chrono::high_resolution_clock::now();
    string fout_name = "/sys/fs/cgroup/new_SF100/partition_result_all_" + to_string(partition_cnt);
    fout_name = fout_name + "_v2.txt";
    string fout1_name = "/sys/fs/cgroup/new_SF100/region_node_component_" + to_string(partition_cnt);
    fout1_name = fout1_name + "_v2.txt";
	fout.open(fout_name);
    fout1.open(fout1_name);
    sort(re, re + partition_cnt, cmp2);
    for (int i = 0; i < partition_cnt; i++){
        fout << "partition_id: " << i << endl;
		fout << "partition_size: " << re[i].cost << endl;
		fout << "partition_element: ";
        for (int j = 0; j < partition_component[i].size(); j++) {
            fout << partition_component[i][j] << ", ";
        }
        fout << endl;
    }
    for (int i = 0; i < partition_cnt; i++) {
        fout1 << "partition_id: " << i << endl;
        fout1 << "partition_size: " << partition_node_component[i].size() - 1 << endl;
        fout1 << "partition_node_element: ";
        for (int j = 1; j < partition_node_component[i].size(); j++) {
            fout1 << partition_node_component[i][j] << ",";
        }
        fout1 << endl;
    }

    end = std::chrono::high_resolution_clock::now();

    duration = end - start;

    // fout2 << "Time taken by the code: " << duration.count() << " ms" << endl;

	return 0;
}
