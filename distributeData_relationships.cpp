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

vector<vector<int>> conn_list;
vector<string> label_list;
vector<vector<int>> connected_component_list;
vector<string> relationship_line;

void Stringsplit(string str, const char split,vector<string>& res)
{
	istringstream iss(str);
	string token;	
	while (getline(iss, token, split))
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
		
            if (filename == "." || filename == "..") {
                continue;
            }

            std::string full_path = path + "/" + filename;
            if (entry->d_type == DT_DIR) {
                get_need_file(full_path, paths, extension);
            } else if (entry->d_type == DT_REG) {
                if (filename.substr(filename.find_last_of(".") + 1) == extension) {
                    paths.push_back(full_path);
                }
            }
        }
        closedir(dir);
    }
}

std::string toLowercase(std::string str) {
    for (char &c : str) {
        c = std::tolower(static_cast<unsigned char>(c));
    }
    return str;
}

int main(){
    ifstream fin1("/sys/fs/cgroup/new_SF100/partition_result_all_8_v2.txt");
    string line;
    int line_cnt = 0;
    while(getline(fin1, line)){
        line_cnt++;
        if(line_cnt % 3 != 0){
            continue;
        }
        int pos = line.find(":");
	string tline = line.substr(pos + 1);
        vector<int> cc_id;
        vector<string> strList;
        Stringsplit(tline, ',', strList);
        for (int i = 0; i < strList.size(); i++) {
            if (strList[i] == " ") {
                continue;
            }
            int tn = stoi(strList[i]);
            cc_id.push_back(tn);
        }
        conn_list.push_back(cc_id);
    }

    ifstream fin2("/sys/fs/cgroup/new_SF100/connected_label_v4_clear.txt");
    string cc_line;
    label_list.push_back("NULL");
    while(getline(fin2, cc_line)){
        int pos = cc_line.find(":");
        string cc_label = cc_line.substr(pos + 1);
        label_list.push_back(cc_label);
    }

    ifstream fin3("/sys/fs/cgroup/new_SF100/region_component_v4.txt");
    string node_line;
    int cc_cnt = 0;
    vector<int> tini;
    tini.push_back(0);
    connected_component_list.push_back(tini);
    while(getline(fin3, node_line)){
        cc_cnt++;
        int pos = node_line.find(":");
	string tline = node_line.substr(pos + 1);
        vector<int> node_id;
        vector<string> strList;
        Stringsplit(tline, ',', strList);
        for (int i = 0; i < strList.size(); i++) {
            int tn = stoi(strList[i]);
            node_id.push_back(tn);
        }
	connected_component_list.push_back(node_id);
    }

    ifstream fin4("/sys/fs/cgroup/new_SF100/new_output_relationships.txt");
    string r_line;
    long long r_cnt = 0;
    while (getline(fin4, r_line)) {
        relationship_line.push_back(r_line);
        r_cnt++;
    }

    int now_partition_id = 0;
    string file_path = "/sys/fs/cgroup/new_SF100/new_output_relationships";
    vector<string> my_file, file_list;
	string need_extension = "txt";
	get_need_file(file_path, my_file, need_extension);

    for (int i = 0; i < my_file.size(); i++){
        string tline = my_file[i];
        int pos = tline.find("relationships");
        tline = tline.substr(pos + 14);
        pos = tline.find(".");
        tline = tline.substr(0, pos)
        file_list.push_back(tline);
    }
    while (now_partition_id < 8){
        for (int i = 0; i < my_file.size(); i++){
            set<int> conn_id_list;
            for (int j = 0; j < conn_list[now_partition_id].size(); j++){
                int bh = conn_list[now_partition_id][j];
                if (label_list[bh] == file_list[i]){
                    for (int k = 0; k < connected_component_list[bh].size(); k++){
                        conn_id_list.insert(connected_component_list[bh][k]);
                    }
                }
            }
            if (conn_id_list.size() == 0) {
                continue;
            }
            ifstream tfin(my_file[i]);
            ofstream tout1, tout2, tout3, tout4;
            int out_file_cnt = 1;
            string re_line;
            string label1 = "";
            string label2 = "";
            int has_init = 0;
            while (getline(tfin, re_line)) {
                if (has_init == 0) {
                    int pos = re_line.find(' ');
                    string tline = re_line.substr(pos + 1);
                    pos = tline.find(".id");
                    label1 = tline.substr(0, pos);
                    pos = tline.find(' ');
                    tline = tline.substr(pos + 1);
                    pos = tline.find(".id");
                    label2 = tline.substr(0, pos);
                    label1 = toLowercase(label1);
                    label2 = toLowercase(label2);
                    string fout_name = "/sys/fs/cgroup/new_SF100/partition_txt/partition_" + to_string(now_partition_id) + "/" + label1 + "_" + file_list[i] + "_" + label2 + ".txt";
                    tout1.open(fout_name);
                    has_init = 1;
                }
                int tpos = re_line.find(' ');
                string ttline = re_line.substr(tpos + 1);
                tpos = ttline.find(".id");
                string tlabel1 = ttline.substr(0, tpos);
                tpos = ttline.find(' ');
                ttline = ttline.substr(tpos + 1);
                tpos = ttline.find(".id");
                string tlabel2 = ttline.substr(0, tpos);
                tlabel1 = toLowercase(tlabel1);
                tlabel2 = toLowercase(tlabel2);
                if (tlabel1 != label1 || tlabel2 != label2) {
                    out_file_cnt++;
                    label1 = tlabel1;
                    label2 = tlabel2;
                    string fout_name = "/sys/fs/cgroup/new_SF100/partition_txt/partition_" + to_string(now_partition_id) + "/" + label1 + "_" + file_list[i] + "_" + label2 + ".txt";
                    if (out_file_cnt == 2) {
                        tout2.open(fout_name);
                    }
                    if (out_file_cnt == 3) {
                        tout3.open(fout_name);
                    }
                    if (out_file_cnt == 4) {
                        tout4.open(fout_name);
                    }
                }
                tpos = re_line.find(".id:");
                string treline = re_line.substr(tpos + 4);
                tpos = treline.find(' ');
                string tid1 = treline.substr(0, tpos);
                treline = treline.substr(tpos + 1);
                tpos = treline.find(".id:");
                treline = treline.substr(tpos + 4);
                tpos = treline.find(' ');
                string tid2 = treline.substr(0, tpos);
                int txtid1 = stoi(tid1);
                int txtid2 = stoi(tid2);
                set<int>::iterator it1 = conn_id_list.find(txtid1);
                set<int>::iterator it2 = conn_id_list.find(txtid2);
                if (it1 != conn_id_list.end() && it2 != conn_id_list.end()) {
                    int pos = re_line.find(":");
                    string teline = re_line.substr(pos + 1);
                    pos = teline.find(' ');
                    teline = teline.substr(0, pos);
                    int reid = stoi(teline);
                    string temp_line = relationship_line[reid];
                    if (out_file_cnt == 1) {
                        tout1 << temp_line << endl;
                    }
                    if (out_file_cnt == 2) {
                        tout2 << temp_line << endl;
                    }
                    if (out_file_cnt == 3) {
                        tout3 << temp_line << endl;
                    }
                    if (out_file_cnt == 4) {
                        tout4 << temp_line << endl;
                    }
                }
            }
        }
        now_partition_id++;
    }

    return 0;
}
