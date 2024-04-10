#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <dirent.h>
#include <regex>
#include <map> 

#define ll long long

using namespace std;

//将source根据指定分隔符delimiter分割储存到vect
void stringSplit1(const string &source, const string &delimiter, vector<string> &vect) {
	// 子串首
	string::size_type pos1;
	// 子串尾
	string::size_type pos2;
	// pos2初始化成第一个分隔符
	pos2 = source.find(delimiter);
	// pos1初始化为0
	pos1 = 0;
	
	while (string::npos != pos2) {
		vect.push_back(source.substr(pos1, pos2 - pos1));
		pos1 = pos2 + delimiter.size();
		pos2 = source.find(delimiter, pos1);
	}

	//如果分隔符没在尾部，补上尾部子串
	if (pos1 != source.length()) {
		vect.push_back(source.substr(pos1));
	}
}

//找到指定目录下的特定扩展名文件的路径存入file
void get_need_file(const std::string& path, std::vector<std::string>& paths, const std::string& extension) {
    DIR* dir;
    struct dirent* entry;

	cout<<"----finding in path: "<<path<<"to find extension: "<<extension<<"----"<<endl;

    if ((dir = opendir(path.c_str())) != nullptr) {
        while ((entry = readdir(dir)) != nullptr) {
            std::string filename = entry->d_name;

			cout<<"file name:"<<filename<<endl;

            // 忽略当前目录和上级目录的项
            if (filename == "." || filename == "..") {
                continue;
            }

            std::string full_path = path + "/" + filename;

            if (entry->d_type == DT_DIR) {  // 如果是目录，则递归遍历子目录
                get_need_file(full_path, paths, extension);
            } else if (entry->d_type == DT_REG) {  // 只处理普通文件
                if (filename.substr(filename.find_last_of(".") + 1) == extension) {  // 根据后缀进行筛选
                    paths.push_back(full_path);
					cout<<"find file path:"<<full_path<<endl;
                }
            }
        }
        closedir(dir);
    }

	cout<<"--------------stop find file path in"<<path<<"--------------"<<endl;
}

struct item {
	int new_id;
	string old_id;
	string label;
};

int main() {
	string file_path = "./import100";
	vector<string> my_file;//文件路径向量组
	get_need_file(file_path, my_file, "csv");

	ofstream fout1;
	fout1.open("/sys/fs/cgroup/new_SF100/new_output_nodes.txt");//节点重编号总文件
	ofstream fout2;
	fout2.open("/sys/fs/cgroup/new_SF100/new_output_relationships.txt");//属性边重编号总文件

	vector<item> graph;//储存所有节点信息的新id、老id和label
	string s;
	ll nodeID = 0;
	ll relationshipID = 0;

	map<string,ll>q;//用于快速查询graph下标，key是老id+label，value是graph下标
	
	//处理所有csv文件中的node文件
	for (ll i = 0; i < my_file.size(); i++) {
		ifstream fin(my_file[i]);
		//如果路径中只有两个_，说明是节点文件
		if (count(my_file[i].begin(), my_file[i].end(), '_') == 2) {
			cout << "File " << i + 1 << " is Node File:" << endl;
			cout << my_file[i] << endl;

			//处理标题行
			getline(fin, s);
			vector<string> vec;
			stringSplit1(s, "|", vec);
			size_t j = 0;
			size_t vecSize = vec.size();
			string vecLabel[20];
			for (j = 0; j < vec.size(); j++) {
				vecLabel[j] = vec[j];
			}
			

			int positionA = my_file[i].find('_', 0);
			int positionB = my_file[i].rfind('/');
			//cout << position << endl;
			string label = my_file[i].substr(positionB+1, positionA-positionB-1);
			cout << "label:" <<label << endl;

    		//处理数据行
			while (getline(fin, s)) {
				vector<string> vec;
				stringSplit1(s, "|", vec);
				//cout << "source " << s << ":" << endl << "split: ";
				size_t j = 0;
				size_t vecSize = vec.size();

				item temp;
				temp.new_id = nodeID;
				temp.label = label;//节点名 
				temp.old_id = vec[0]; 
				graph.push_back(temp);
				q[vec[0]+label]=nodeID; 
				
				//向节点总文件登入该node
				fout1 << "label:" << label << " ";
				fout1 << vecLabel[0] << ":" << nodeID << " ";
				for (j = 1; j < vecSize; j++) {
					fout1 << vecLabel[j] << ":" << vec[j] << " ";
				}
				fout1 << endl;
				nodeID++;
			}
		}
	}
	
	cout<<"---------sizeof q: "<<q.size()<<"------------"<<endl;

	//处理所有csv文件中的relatisonship文件
	for (ll i = 0; i < my_file.size(); i++) {
		ifstream fin(my_file[i]);
		//如果有3个及以上的_说明是属性边文件
		if (count(my_file[i].begin(), my_file[i].end(), '_') >= 3) {
			cout << "File " << i + 1 << " is Relationship File:" << endl;
			cout << my_file[i] << endl;

			string relationship_type;

			int pos1 = my_file[i].find("/", 2);
			relationship_type = my_file[i].substr(pos1 + 1); 

			int pos2 = relationship_type.find("_");
			int pos3 = relationship_type.find("_", pos2 + 1);
			relationship_type = relationship_type.substr(pos2 + 1, pos3 - pos2 - 1);
			cout << relationship_type;
			string output = "/sys/fs/cgroup/new_SF100/new_output_relationships/";

			ofstream fout;//单个属性边的txt文件
			fout.open(output.append(relationship_type).append(".txt"), ofstream::app);

			//处理标题行的前两项
			getline(fin, s);
			vector<string> vec;
			stringSplit1(s, "|", vec);
			size_t j=0;
			size_t vecSize = vec.size();
			vector<string> vecLabel;
			string nodeLabel[10];
			for (j = 0; j < vecSize; j++) {
				vecLabel.push_back(vec[j]);
				//cout<<j<<"|"<<vecLabel[j]<<"|"<<vec[j]<<endl;
			}
			for (j = 0; j < vecSize; j++) {
				if (j < 2) {
					int position = vecLabel[j].find('.'); //person.id
					nodeLabel[j] = vecLabel[j].substr(0, position);
				}
				else{
					nodeLabel[j] = vecLabel[j];
				}
				
				for (int k = 0; k < nodeLabel[j].size(); k++) {
					nodeLabel[j][k] = tolower(nodeLabel[j][k]);
				}
			}
			
			//处理剩余数据行
			while (getline(fin, s)) {
				vector<string> vec;
				stringSplit1(s, "|", vec);
				size_t j = 0;
				size_t vecSize = vec.size();
				//将该边信息登入总边txt文件
				fout2 << "type:" << relationship_type << " ";
				fout2 << "id:" << relationshipID << " ";
				//登入该属性txt文件
				fout << "id:" << relationshipID << " ";
				//查q将新id登入总边txt文件
				for (j = 0; j < vecSize; j++) {
					if (q.count(vec[j]+nodeLabel[j])!=1){
						cout<<"erro: q has no key "<<vec[j]+nodeLabel[j]<<endl;
					}
					int k=q[vec[j]+nodeLabel[j]];
					fout2 << vecLabel[j] << ":" << graph[k].new_id << " ";
					fout << vecLabel[j] << ":" << graph[k].new_id << " ";
				}
				
				for (; j < vecSize; j++){
					fout2 << vecLabel[j] << ":" << vec[j] << " ";
				}
				fout2 << endl;
				fout << endl;
				relationshipID++;
			}
		}
		fin.close();
	}
	if (my_file.size() == 0) {
		cout << "No file can be found!" << endl;
	} else {
		cout << endl << "Process " << my_file.size() << " file(s)." << endl;
	}
	return 0;
}
