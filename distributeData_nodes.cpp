#include <iostream>
#include <cstring>
#include <algorithm>
#include <vector>
#include <set>
#include <fstream>
#include <sstream>
#include <sys/io.h>
#include <dirent.h>
using namespace std;

vector<vector<int>> conn_node_list;
vector<vector<int>> connected_component_list;
struct Node {
    string label;
    int id;
    string comment;
}node[288900000];


void Stringsplit(string str, const char split,vector<string>& res)
{
	istringstream iss(str);
	string token;
	while (getline(iss, token, split) {
	{
		res.push_back(token);
	}
}

void get_need_file(const std::string& path, std::vector<std::string>& paths, const std::string& extension) {
    DIR* dir;
    struct dirent* entry;

	cout<<"----finding in path: "<<path<<"to find extension: "<<extension<<"----"<<endl;

    if ((dir = opendir(path.c_str())) != nullptr) {
        while ((entry = readdir(dir)) != nullptr) {
            std::string filename = entry->d_name;

			// cout<<"file name:"<<filename<<endl;

            if (filename == "." || filename == "..") {
                continue;
            }

            std::string full_path = path + "/" + filename;
            if (entry->d_type == DT_DIR) {
                get_need_file(full_path, paths, extension);
            } else if (entry->d_type == DT_REG) {
				// cout << filename << endl;
				// cout << filename.substr(filename.find_last_of(".") + 1) <<endl;
                if (filename.substr(filename.find_last_of(".") + 1) == extension) {
					// cout << 123 << endl;
                    paths.push_back(full_path);
                }
            }
        }
        closedir(dir);
    }
}

int main(){
    ifstream fin1("/sys/fs/cgroup/new_SF100/region_node_component_8_v2.txt");
    string line;
    int line_cnt = 0;
    while(getline(fin1, line)){
        line_cnt++;
        if(line_cnt % 3 != 0){
            continue;
        }
        int pos = line.find(":");
	string tline = line.substr(pos + 1);
        vector<int> node_id;
        vector<string> strList;
        Stringsplit(tline, ',', strList);
        for (int i = 0; i < strList.size(); i++) {
            // cout << "strList[i]: " << strList[i] << " " << strList[i].length() << endl;
            if (strList[i] == " " || strList[i].length() < 1) {
                continue;
            }
            int tn = stoi(strList[i]);
            node_id.push_back(tn);
        }
        conn_node_list.push_back(node_id);
    }
    // cout << "line_cnt: " << line_cnt << endl;

    ifstream fin2("/sys/fs/cgroup/new_SF100/new_output_nodes.txt");
    string node_line;
    int node_cnt = 0;
    while(getline(fin2, node_line)){
        node[node_cnt].id = node_cnt;
        node[node_cnt].comment = node_line;

        int label_pos = node_line.find("label:");
        string tlabel = node_line.substr(label_pos + 6);
        label_pos = tlabel.find(" ");
		tlabel = tlabel.substr(0, label_pos);
        node[node_cnt].label = tlabel;
        node_cnt++;
    }

    int now_partition_id = 0;
    string file_label_list[8] = {"post", "comment", "forum", "person", "place", "organisation", "tag", "tagclass"};
    
    while (now_partition_id < 8){
        ofstream fout0, fout1, fout2, fout3, fout4, fout5, fout6, fout7, fout8;
        string fout_name0 = "/sys/fs/cgroup/new_SF100/partition_txt/partition_" + to_string(now_partition_id) + "/" + file_label_list[0] + "_0_0.txt";
        string fout_name1 = "/sys/fs/cgroup/new_SF100/partition_txt/partition_" + to_string(now_partition_id) + "/" + file_label_list[1] + "_0_0.txt";
        string fout_name2 = "/sys/fs/cgroup/new_SF100/partition_txt/partition_" + to_string(now_partition_id) + "/" + file_label_list[2] + "_0_0.txt";
        string fout_name3 = "/sys/fs/cgroup/new_SF100/partition_txt/partition_" + to_string(now_partition_id) + "/" + file_label_list[3] + "_0_0.txt";
        string fout_name4 = "/sys/fs/cgroup/new_SF100/partition_txt/partition_" + to_string(now_partition_id) + "/" + file_label_list[4] + "_0_0.txt";
        string fout_name5 = "/sys/fs/cgroup/new_SF100/partition_txt/partition_" + to_string(now_partition_id) + "/" + file_label_list[5] + "_0_0.txt";
        string fout_name6 = "/sys/fs/cgroup/new_SF100/partition_txt/partition_" + to_string(now_partition_id) + "/" + file_label_list[6] + "_0_0.txt";
        string fout_name7 = "/sys/fs/cgroup/new_SF100/partition_txt/partition_" + to_string(now_partition_id) + "/" + file_label_list[7] + "_0_0.txt";
        fout0.open(fout_name0);
        fout1.open(fout_name1);
        fout2.open(fout_name2);
        fout3.open(fout_name3);
        fout4.open(fout_name4);
        fout5.open(fout_name5);
        fout6.open(fout_name6);
        fout7.open(fout_name7);
        for (int i = 0; i < conn_node_list[now_partition_id].size(); i++) {
            int bh = conn_node_list[now_partition_id][i];
            if (node[bh].label == file_label_list[0]) {
                fout0 << node[bh].comment << endl;
            }
            if (node[bh].label == file_label_list[1]) {
                fout1 << node[bh].comment << endl;
            }
            if (node[bh].label == file_label_list[2]) {
                fout2 << node[bh].comment << endl;
            }
            if (node[bh].label == file_label_list[3]) {
                fout3 << node[bh].comment << endl;
            }
            if (node[bh].label == file_label_list[4]) {
                fout4 << node[bh].comment << endl;
            }
            if (node[bh].label == file_label_list[5]) {
                fout5 << node[bh].comment << endl;
            }
            if (node[bh].label == file_label_list[6]) {
                fout6 << node[bh].comment << endl;
            }
            if (node[bh].label == file_label_list[7]) {
                fout7 << node[bh].comment << endl;
            }
        }
        now_partition_id++;
    }

    return 0;
}
