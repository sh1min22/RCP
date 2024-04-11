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
            tokens.back() += " " + token;
        } else {
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
    for (size_t i = 0; i < values.size(); ++i) {
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
    std::ifstream inFile(filename);
    string clear_filename = getFileName(filename);

    string out_filename = "/sys/fs/cgroup/new_SF100/partition_csv/partition_" + to_string(now_partition_id) + "/" + clear_filename + ".csv";
    std::ofstream outFile(out_filename);

    outFile.imbue(std::locale("C.UTF-8"));

    std::string line;
    std::vector<std::string> headers;

    if (std::getline(inFile, line)) {
        std::vector<std::string> keyValuePairs = split(line);
        for (const auto& pair : keyValuePairs) {
            std::vector<std::string> keyValuePair = splitKeyValuePair(pair);
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

    processNodeLine(line, headers, outFile);

    while (std::getline(inFile, line)) {
        processNodeLine(line, headers, outFile);
    }
}

void relationship2Csv(string filename, int now_partition_id) {
    std::ifstream inFile(filename);
    string clear_filename = getFileName(filename);

    string out_filename = "/sys/fs/cgroup/new_SF100/partition_csv/partition_" + to_string(now_partition_id) + "/" + clear_filename + "_0_0.csv";
    std::ofstream outFile(out_filename);

    std::string line;
    std::vector<std::string> headers;

    if (std::getline(inFile, line)) {
        std::vector<std::string> keyValuePairs = split(line);
        for (const auto& pair : keyValuePairs) {
            std::vector<std::string> keyValuePair = splitKeyValuePair(pair);
            if (keyValuePair.size() == 2 && keyValuePair[0] != "id" && keyValuePair[0] != "type") {
                headers.push_back(keyValuePair[0]);
            }
        }
        std::replace(headers[0].begin(), headers[1].end(), '.', '_');
        std::replace(headers[1].begin(), headers[1].end(), '.', '_');
        if (headers[0] == headers[1]) {
            headers[0] = headers[0] + '1';
            headers[1] = headers[1] + '2';
        }
        for (size_t i = 0; i < headers.size(); ++i) {
            outFile << headers[i];
            if (i < headers.size() - 1) {
                outFile << ",";
            }
        }
        outFile << "\n";
    }

    processRelationshipLine(line, headers, outFile);

    while (std::getline(inFile, line)) {
        processRelationshipLine(line, headers, outFile);
    }
}

int main() {
    for (int i = 0; i < 8; i++) {
        string file_path = "/sys/fs/cgroup/new_SF100/partition_txt/partition_" + to_string(i);
        vector<string> my_file;
        string need_extension = "txt";
        get_need_file(file_path, my_file, need_extension);
        for (int j = 0; j < my_file.size(); j++) {
            if (my_file[j].find("_0_0") != -1) {
                node2Csv(my_file[j], i);
            }
            else{
                relationship2Csv(my_file[j], i);
            }
        }
    }
	return 0;
}
