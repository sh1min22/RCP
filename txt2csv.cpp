#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <sys/io.h>
#include <dirent.h>
#include <cctype>
#include <typeinfo>

using namespace std;

int ccnt = 1;
int line_cnt = 0;

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

string getFileName(string origin_name) {
    string tline = origin_name;
    int pos = tline.find("partition_");
    tline = tline.substr(pos + 14);
    pos = tline.find("partition_");
    tline = tline.substr(pos + 12);
    pos = tline.find(".");
    tline = tline.substr(0, pos);
    return tline;
}

std::string toLowercase(std::string str) {
    for (char &c : str) {
        c = std::tolower(static_cast<unsigned char>(c));
    }
    return str;
}

// 分割字符串函数，处理冒号分隔的键值对，同时处理特殊情况（如URL）
std::vector<std::string> splitKeyValuePair(const std::string &s) {
    size_t pos = s.find(':');
    if (pos != std::string::npos) {
        return {s.substr(0, pos), s.substr(pos + 1)};
    }
    return {};
}

std::vector<std::string> split(const std::string &s) {
    std::vector<std::string> tokens;
    std::istringstream tokenStream(s);
    std::string token;

    while (std::getline(tokenStream, token, ' ')) {
        if (!tokens.empty() && token.find(':') == std::string::npos) {
            // 如果当前token中没有冒号，将其加到前一个token中
            tokens.back() += " " + token;
        } else {
            // 否则，作为一个新的token添加
            tokens.push_back(token);
        }
    }

    return tokens;
}

void processNodeLine(const std::string& line, const std::vector<std::string>& headers, std::ofstream& outFile) {
    std::vector<std::string> keyValuePairs = split(line);
    std::vector<std::string> values(headers.size(), "");

    for (const auto& pair : keyValuePairs) {
        std::vector<std::string> keyValuePair = splitKeyValuePair(pair);
        if (keyValuePair.size() == 2) {
            auto it = std::find(headers.begin(), headers.end(), keyValuePair[0]);
            if (it != headers.end() && keyValuePair[0] != "label") {
                if (keyValuePair[0] == "id") {
                    // cout << "上: " << keyValuePair[1] << endl;
                    if (std::isdigit(keyValuePair[1][0])) {
                        values[it - headers.begin()] = keyValuePair[1];
                    }
                    else {
                        continue;
                    }
                }
                else {
                    values[it - headers.begin()] = keyValuePair[1];
                }
            }
        }
    }
    // cout << "values:" << values.size() << endl;
    for (size_t i = 0; i < values.size(); ++i) {
        // std::cout << "类型 of myInt: " << typeid(values[i]).name() << std::endl;
        outFile << values[i];
        if (i < values.size() - 1) {
            outFile << ",";
        }
    }
    outFile << "\n";
}

void processRelationshipLine(const std::string& line, const std::vector<std::string>& headers, std::ofstream& outFile) {
    std::vector<std::string> keyValuePairs = split(line);
    std::vector<std::string> values(headers.size(), "");
    int now_pos = 0;
    for (const auto& pair : keyValuePairs) {
        std::vector<std::string> keyValuePair = splitKeyValuePair(pair);
        // cout << "relationship_keyValuePair[0]: " << keyValuePair[0] << endl;
        // cout << "relationship_keyValuePair[0]: " << keyValuePair[1] << endl;
        if (keyValuePair.size() == 2) {
            if (keyValuePair[0] != "id" && keyValuePair[0] != "type") {
                values[now_pos] = keyValuePair[1];
                now_pos++;
            }
        }
    }
    for (size_t i = 0; i < values.size(); ++i) {
        outFile << values[i];
        if (i < values.size() - 1) {
            outFile << ",";
        }
    }
    outFile << "\n";
}

void node2Csv(string filename, int now_partition_id) {
    cout << "filename: " << filename << endl;
    std::ifstream inFile(filename); // 输入文件
    string clear_filename = getFileName(filename);

    // int pos = clear_filename.find("_0_0");
    // clear_filename = clear_filename.substr(0, pos);
    // cout << "clear_filename: " << clear_filename << endl;
    // if (clear_filename != "comment_0_0") {
        // return ;
    // }

    string out_filename = "/sys/fs/cgroup/new_SF100/partition_csv/partition_" + to_string(now_partition_id) + "/" + clear_filename + ".csv";
    cout << "out_filename: " << out_filename << endl;
    std::ofstream outFile(out_filename); // 输出文件

    outFile.imbue(std::locale("C.UTF-8"));

    std::string line;
    std::vector<std::string> headers;

    // 读取第一行来创建表头
    if (std::getline(inFile, line)) {
        cout << "line:" << line << endl;
        std::vector<std::string> keyValuePairs = split(line);
        cout << keyValuePairs.size() << endl;
        for (const auto& pair : keyValuePairs) {
            std::vector<std::string> keyValuePair = splitKeyValuePair(pair);
            // cout << keyValuePair[0] << " " << keyValuePair[1] << endl;
            if (keyValuePair.size() == 2 && keyValuePair[0] != "label") {
                headers.push_back(keyValuePair[0]);
            }
        }

        for (size_t i = 0; i < headers.size(); ++i) {
            outFile << headers[i];
            if (i < headers.size() - 1) {
                outFile << ",";
            }
        }
        outFile << "\n";
    }

    // 重新处理第一行数据
    processNodeLine(line, headers, outFile);

    // 处理剩余行
    while (std::getline(inFile, line)) {
        // cout << line << endl;
        processNodeLine(line, headers, outFile);
    }
}

void relationship2Csv(string filename, int now_partition_id) {
    // cout << "filename: " << filename << endl;
    std::ifstream inFile(filename); // 输入文件
    string clear_filename = getFileName(filename);
    // cout << "clear_filename:" << clear_filename << endl;

    // int pos = clear_filename.find("comment_hasTag_tag");
    // if (pos == -1) {
    //     return ;
    // }

    string out_filename = "/sys/fs/cgroup/new_SF100/partition_csv/partition_" + to_string(now_partition_id) + "/" + clear_filename + "_0_0.csv";
    // cout << "out_filename: " << out_filename << endl;
    std::ofstream outFile(out_filename); // 输出文件

    std::string line;
    std::vector<std::string> headers;

    if (std::getline(inFile, line)) {
        // cout << "line:" << line << endl;
        std::vector<std::string> keyValuePairs = split(line);
        for (const auto& pair : keyValuePairs) {
            std::vector<std::string> keyValuePair = splitKeyValuePair(pair);
            // cout << keyValuePair[0] << " " << keyValuePair[1] << endl;
            if (keyValuePair.size() == 2 && keyValuePair[0] != "id" && keyValuePair[0] != "type") {
                headers.push_back(keyValuePair[0]);
            }
        }
        // cout << headers[0] << " " << headers[1] << " " << headers[2] <<endl;
        std::replace(headers[0].begin(), headers[1].end(), '.', '_');
        std::replace(headers[1].begin(), headers[1].end(), '.', '_');
        if (headers[0] == headers[1]) {
            headers[0] = headers[0] + '1';
            headers[1] = headers[1] + '2';
        }
        cout << headers[0] << " " << headers[1] << " " << headers.size() << endl;
        for (size_t i = 0; i < headers.size(); ++i) {
            // cout << headers[i] << endl;
            outFile << headers[i];
            if (i < headers.size() - 1) {
                outFile << ",";
            }
        }
        outFile << "\n";
    }

    // 重新处理第一行数据
    processRelationshipLine(line, headers, outFile);

    // 处理剩余行
    while (std::getline(inFile, line)) {
        // cout << line << endl;
        processRelationshipLine(line, headers, outFile);
    }
}

int main() {
    for (int i = 0; i < 8; i++) {
        cout << "now_partition_id:" << i << endl;
        string file_path = "/sys/fs/cgroup/new_SF100/partition_txt/partition_" + to_string(i);
        vector<string> my_file;
        string need_extension = "txt";
        get_need_file(file_path, my_file, need_extension);
        cout << "my_file: " << my_file.size() << endl;
        for (int j = 0; j < my_file.size(); j++) {
            if (my_file[j].find("_0_0") != -1) {
                // continue;
                cout << "Node_file " << j + 1 << " is:" << my_file[j] << endl;
                node2Csv(my_file[j], i);
            }
            else{
                // continue;
                cout << "Relationship_file " << j + 1 << " is:" << my_file[j] << endl;
                relationship2Csv(my_file[j], i);
            }
        }
        // break;
    }
	return 0;
}