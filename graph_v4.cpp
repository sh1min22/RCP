#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sys/io.h>
#include <dirent.h>
#include <chrono>

using namespace std;

vector<int>parent;
vector<int>rank1;
ofstream fout, fout1, fout2, fout3;
typedef pair<int, vector<int> > PII;

int node_cnt[282640000];

int ccnt = 1;

// int property_num[] = {5, 2, 3, 7, 3, 7, 2, 2};
// int new_property_num[] = {8, 6, 8, 3, 4, 3, 3, 4};
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

void get_need_file(const std::string& path, std::vector<std::string>& paths, const std::string& extension) {
    DIR* dir;
    struct dirent* entry;

	cout<<"----finding in path: "<<path<<"to find extension: "<<extension<<"----"<<endl;

    if ((dir = opendir(path.c_str())) != nullptr) {
        while ((entry = readdir(dir)) != nullptr) {
            std::string filename = entry->d_name;

			// cout<<"file name:"<<filename<<endl;

            // 忽略当前目录和上级目录的项
            if (filename == "." || filename == "..") {
                continue;
            }

            std::string full_path = path + "/" + filename;
            if (entry->d_type == DT_DIR) {  // 如果是目录，则递归遍历子目录
                get_need_file(full_path, paths, extension);
            } else if (entry->d_type == DT_REG) {  // 只处理普通文件
				// cout << filename << endl;
				// cout << filename.substr(filename.find_last_of(".") + 1) <<endl;
                if (filename.substr(filename.find_last_of(".") + 1) == extension) {  // 根据后缀进行筛选
					// cout << 123 << endl;
                    paths.push_back(full_path);
                }
            }
        }
        closedir(dir);
    }
}

void DisJointSetUnion(int n) {
	for (int i = 0; i < n; i++) {
		parent.push_back(i);
		rank1.push_back(1);
	}
}

int find(int x) {
	if (x == parent[x])
		return x;
	else {
		parent[x] = find(parent[x]);
		return parent[x];
	}
}

void Union(int x, int y) {
	int rx = find(x);
	int ry = find(y);

	if (rx != ry) {
		if (rank1[rx] < rank1[ry]) {
			swap(rx, ry);
		}

		parent[ry] = rx;

		if (rank1[rx] == rank1[ry])
			rank1[rx] += 1;
	}
}


void stringSplit1(const string &source, const string &delimiter, vector<string> &vect) {
	string::size_type pos1;
	string::size_type pos2;

	pos2 = source.find(delimiter);
	pos1 = 0;
	while (string::npos != pos2) {
		vect.push_back(source.substr(pos1, pos2 - pos1));

		pos1 = pos2 + delimiter.size();
		pos2 = source.find(delimiter, pos1);
	}
	if (pos1 != source.length()) {
		vect.push_back(source.substr(pos1));
	}
}

int readline(string strPath, int &line) {
	ifstream is(strPath);
	char c;
	while (is.get(c)) {
		if (c == '\n')
			line++;
	}
	is.close();
	//cout << "line=" << line << endl;
	return line;
}

string getFileName(string origin_name) {
    string tline = origin_name;

	int pos = tline.find("relationships");
	tline = tline.substr(pos + 14);

    // int pos = tline.find("/");
    // tline = tline.substr(pos + 1);
    // pos = tline.find("/");
    // tline = tline.substr(pos + 1);
    pos = tline.find(".");
    tline = tline.substr(0, pos);
    return tline;
}

int getRelationshipsCost(int node_id, string file_name) {
    if (file_name == "hasMember" || file_name == "knows" || file_name == "likes" || file_name == "studyAt" || file_name == "workAt") {
        return 2 * node_cnt[node_id];
    }
    else{
        return node_cnt[node_id];
    }
}

void create_Connected(string filename) {
    memset(node_cnt, 0 ,sizeof(node_cnt));
	fout2 << filename << endl;
	vector<PII> nodes_parent;

	string file_name = filename;
	ifstream fin_relationships(file_name);
	ifstream fin_relationships2(file_name);
	vector<int> nodes;

    // string file_name_clear = getFileName(file_name);
	// cout << "file_name_clear: " << file_name_clear << endl;
	// while(1) {

	// }

	int line = 0;
	int r_num = readline(file_name, line);
	cout << line << " " << r_num << endl;

	int n_num = 0;
	string s;
	while (getline(fin_relationships, s)) {
		vector<string> vec;
		vector<string> node_u;
		vector<string> node_v;
		stringSplit1(s, " ", vec);
//		cout << "vec:" << vec[2] << endl;
		stringSplit1(vec[1], ":", node_u);
		//cout << node[1];
		int u = atoi(node_u[1].c_str());
		stringSplit1(vec[2], ":", node_v);
		int v = atoi(node_v[1].c_str());
		nodes.push_back(u);
		nodes.push_back(v);
        node_cnt[u]++;
		//cout << u << " " << v << endl;
	}
	fin_relationships.close();

	sort(nodes.begin(), nodes.end());
	nodes.erase(unique(nodes.begin(), nodes.end()),  nodes.end());
	n_num = nodes.size();

	DisJointSetUnion(282640000);

	while (getline(fin_relationships2, s)) {
		vector<string> vec;
		vector<string> node_u;
		vector<string> node_v;

		stringSplit1(s, " ", vec);
		stringSplit1(vec[1], ":", node_u);
		//cout << node[1];
		int u = atoi(node_u[1].c_str());
		stringSplit1(vec[2], ":", node_v);
		int v = atoi(node_v[1].c_str());
		//cout << u << "," << v << endl;
		Union(u, v);
	}

	fin_relationships2.close();
	cout << "nodes number:" << n_num << endl;
	//cout << find(12661);
	vector<int> t;
    t.push_back(11);
    for (int i = 0; i < 282640000; i++){
        nodes_parent.push_back({i, t});
    }

	int count = 0;
	for (int i = 0; i < n_num; i++) {
		// cout << "i:" << i << endl;
		int fx = find(nodes[i]);
		// cout << "fx:" << fx << endl;
		if (fx == nodes[i]) {
			count++;
		}
	}

	cout << "count:" << count << endl;
	fout2 << "connected components number:" << count << endl;

	int cnt_has_find_father = 0;
	for (int i = 0; i < n_num; i++) {
		int fx = find(nodes[i]);
		if (i % 100000 == 0) {
			cout << i << endl;
		}
        nodes_parent[fx].second.push_back(nodes[i]);
		// for(int j = 0; j < nodes_parent.size(); j++){
		// 	if (fx == nodes_parent[j].first){
		// 		nodes_parent[j].second.push_back(nodes[i]);
		// 		cnt_has_find_father++;
		// 		break;
		// 	}
		// }
	}
	// cout << "cnt_has_find_father:" << cnt_has_find_father << endl;

	cout << "connected components number:" << count << endl;
    string file_name_clear = getFileName(file_name);
    for (int i = 0; i < 282640000; i++) {
        if (nodes_parent[i].second.size() > 1) {
            cout << "connected component[" << i << "]" << endl;
		    fout << ccnt << " ";
		    fout1 << ccnt << ":";
			fout3 << ccnt << ":" << filename << endl;
            long long cnt = 0;
            for (int j = 1; j < nodes_parent[i].second.size(); j++){
			    fout1 << nodes_parent[i].second[j] << ",";
			    cnt += new_property_num[judge_label(nodes_parent[i].second[j])];
                cnt += getRelationshipsCost(nodes_parent[i].second[j], file_name_clear);
		    }
            fout << cnt << endl;
		    fout1 << endl;
		    ccnt++;
        }
    }
	fout2 << filename << endl;
	nodes_parent.clear();
}

int main() {
    auto start = std::chrono::high_resolution_clock::now();

	fout.open("/sys/fs/cgroup/new_SF100/connected_size_all_v4.txt");
	fout1.open("/sys/fs/cgroup/new_SF100/region_component_v4.txt");
	fout2.open("/sys/fs/cgroup/new_SF100/done_file_name_v4.txt");
	fout3.open("/sys/fs/cgroup/new_SF100/connected_label_v4.txt");
	string file_path = "/sys/fs/cgroup/new_SF100/new_output_relationships";
	vector<string> my_file;
	string need_extension = "txt";
	get_need_file(file_path, my_file, need_extension);
	cout << "my_file: " << my_file.size() << endl;
	for (int i = 0; i < my_file.size(); i++) {
		cout << "File " << i + 1 << " is:" << my_file[i] << endl;
		create_Connected(my_file[i]);
		parent.clear();
		rank1.clear();
	}

    auto end = std::chrono::high_resolution_clock::now();

    // 计算持续时间
    // 我们以毫秒为单位计算时间
    std::chrono::duration<double, std::milli> duration = end - start;

    std::cout << "Time taken by the code: " << duration.count() << " ms" << std::endl;
	return 0;
}