#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <dirent.h>
#include <regex>
#include <map> 

#define ll long long

using namespace std;

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
                if (filename.substr(filename.find_last_of(".") + 1) == extension) {
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
	vector<string> my_file;
	get_need_file(file_path, my_file, "csv");

	ofstream fout1;
	fout1.open("/sys/fs/cgroup/new_SF100/new_output_nodes.txt");
	ofstream fout2;
	fout2.open("/sys/fs/cgroup/new_SF100/new_output_relationships.txt");

	vector<item> graph;
	string s;
	ll nodeID = 0;
	ll relationshipID = 0;

	map<string,ll>q;
	
	for (ll i = 0; i < my_file.size(); i++) {
		ifstream fin(my_file[i]);
		if (count(my_file[i].begin(), my_file[i].end(), '_') == 2) {
			cout << "File " << i + 1 << " is Node File:" << endl;
			cout << my_file[i] << endl;

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

			while (getline(fin, s)) {
				vector<string> vec;
				stringSplit1(s, "|", vec);
				//cout << "source " << s << ":" << endl << "split: ";
				size_t j = 0;
				size_t vecSize = vec.size();

				item temp;
				temp.new_id = nodeID;
				temp.label = label;
				temp.old_id = vec[0]; 
				graph.push_back(temp);
				q[vec[0]+label]=nodeID; 
				
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

	for (ll i = 0; i < my_file.size(); i++) {
		ifstream fin(my_file[i]);
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

			ofstream fout;
			fout.open(output.append(relationship_type).append(".txt"), ofstream::app);

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
			
			while (getline(fin, s)) {
				vector<string> vec;
				stringSplit1(s, "|", vec);
				size_t j = 0;
				size_t vecSize = vec.size();
				fout2 << "type:" << relationship_type << " ";
				fout2 << "id:" << relationshipID << " ";
				fout << "id:" << relationshipID << " ";
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
