#include <iostream>
#include <vector>
#include <map>
#include <dirent.h>
#include <fstream>
#include <list>
#include <time.h>
#include <stdio.h>
#include <algorithm>
#include <cstring>


using namespace std;

typedef pair<string,int> PAIR;
class node;
vector<node*> node_queue;
char *target_folder;
string idx_size;
vector<string> search_patterns;
vector<string> file_name;
map<string, int> result;
map<string,int> temp_count;
const int MAX_NUM = 10*1024*1024;


void modify_file_name_list(char *);
void insert_new_pattern(string, node*);
void build_fail_ptr(node*);
void ac_match(node*, string,string,node*);
void match(node*);

struct cmp_by_value{
    bool operator()(const PAIR& ltr,const PAIR& rtr) {
        if (ltr.second == rtr.second) {
            if (ltr.first.size() <= rtr.first.size()) {
                for (int i = 0; i < ltr.first.size(); ++i) {
                    if(ltr.first[i]==rtr.first[i]){
                        continue;
                    }
                    else{
                        return int(ltr.first[i])<int(rtr.first[i]);
                    }
                }
            } else {
                for (int i = 0; i < rtr.first.size(); ++i) {
                    if(ltr.first[i]==rtr.first[i]){
                        continue;
                    }
                    else{
                        return int(ltr.first[i])<int(rtr.first[i]);
                    }
                }
            }
            return ltr.first.size()<rtr.first.size();
        } else {
            return ltr.second > rtr.second;
        }
    }
};

class node{
public:
    node* next[256]={NULL};
    node* fail;
    string word;
    node();
};
node::node() {
    fail = NULL;
    word = "";
}


void insert_new_pattern(string pattern, node* root){
    node* current = root;
    for(int i=0; i<pattern.size();++i){
        int index = int(pattern[i]);
        if(current->next[index]==NULL){
            current->next[index] = new node();
        }
        current = current->next[index];
    }
    current->word = pattern;
}


void build_fail_ptr(node *root){
    int head = 0, tail = 0;
    root->fail = NULL;
    node_queue.push_back(root);
    head+=1;
    while(head!=tail){
        node *temp = node_queue[tail];
        tail+=1;
        node *previous = NULL;
        for(int i=0;i<256;++i){
            if(temp->next[i]!=NULL){
                if(temp==root){
                    temp->next[i]->fail=root;
                }
                else{
                    previous = temp->fail;
                    while(previous!=NULL){
                        if(previous->next[i]!=NULL){
                            temp->next[i]->fail = previous->next[i];
                            break;
                        }
                        previous = previous->fail;
                    }
                    if(previous==NULL){
                        temp->next[i]->fail=root;
                    }
                }
                node_queue.push_back(temp->next[i]);
                head+=1;
            }
        }
    }
}

void ac_match(node* root, string long_string,string file_name,node* current){
    for(int i=0;i<long_string.size();++i){
        int index = int(long_string[i]);
        if(index<0 || index>256){
            continue;
        }
        if(index>=65 && index<=90){
            index+=32;
        }
        if(current->next[index]==NULL && current!=root){
            while(current->next[index]==NULL && current!=root){
                if(current->word!=""){
                     temp_count[current->word]+=1;
                }
                current = current->fail; 
            }
            if(current->word!=""){
                    temp_count[current->word]+=1;
            }
            if(current->next[index]!=NULL){
                current = current->next[index];
            }
        }else if(current->next[index]==NULL&&current==root){
            continue;
        }
        else {
            node* temp1= current;
            while(temp1->fail!=NULL){
                if(temp1->word!=""){
                    temp_count[temp1->word]+=1;
                }
                temp1 = temp1->fail;
            }
            current = current->next[index];
        }
    }
}


void match(node* root){
    for(int i=0;i<file_name.size();++i) {
        string target_file = string(target_folder) + "/" + file_name[i];
        ifstream input(target_file);
        string long_string;
        node* current = root;
        for(int i = 0; i<search_patterns.size();++i){
            temp_count[search_patterns[i]]=0;
        }
        while(getline(input, long_string)){
             long_string+="\n";
            //long_string = " "+long_string;
            ac_match(root,long_string,file_name[i],current);
        }
        int total = 0;
        for(int i = 0; i<search_patterns.size();++i){
            if(temp_count[search_patterns[i]]==0){
                total = 0;
                break;
            }
            total+=temp_count[search_patterns[i]];
        }
        if(total!=0){
            result[file_name[i]]=total;
        }
        input.close();
    }
}

void modify_file_name_list(char *path){
    DIR *dp;
    struct dirent *dirp;
    dp = opendir(path);
    while((dirp=readdir(dp))!=NULL){
        if((strcmp(dirp->d_name,".")==0)||(strcmp(dirp->d_name,"..")==0)) {
            continue;
        }
        file_name.push_back(dirp->d_name);
    }
    closedir(dp);
}

int main(int argc, char **argv) {
    if (argc < 3) {
        cout << "Usage: a3search <Folder path> <idx_name> <Search patterns> ...." << endl;
        cout << "       a3search <Folder path> <idx_name> -s <index file size> <search patterns> ....." << endl;
        return 0;
    }
    target_folder = argv[1];
    if (string(argv[3]) == "-s") {
          for (int i = 5; i < argc; ++i) {
            string single = string(argv[i]);
            for(int j=0; j<single.size();++j){
                if(int(single[j])>=65 && int(single[j])<=90){
                    single[j]=char(int(single[j])+32);
                }
            }
            search_patterns.push_back(single);
        }
        modify_file_name_list(argv[1]);
        node* root = new node();
        for(int i = 0; i<search_patterns.size();++i){
            insert_new_pattern(search_patterns[i], root);
        }
        build_fail_ptr(root);
        match(root);
        vector<PAIR> file_occr(result.begin(),result.end());
        sort(file_occr.begin(),file_occr.end(),cmp_by_value());
        for(int i=0;i<file_occr.size();++i){
            cout<<file_occr[i].first<<endl;
        }
    }
    else {
        for (int i = 3; i < argc; ++i) {
            string single = string(argv[i]);
            for(int j=0; j<single.size();++j){
                if(int(single[j])>=65 && int(single[j])<=90){
                    single[j]=char(int(single[j])+32);
                }
            }
            search_patterns.push_back(single);
        }
        modify_file_name_list(argv[1]);
        node* root = new node();
        for(int i = 0; i<search_patterns.size();++i){
            insert_new_pattern(search_patterns[i], root);
        }
        build_fail_ptr(root);
        match(root);
        vector<PAIR> file_occr(result.begin(),result.end());
        sort(file_occr.begin(),file_occr.end(),cmp_by_value());
        for(int i=0;i<file_occr.size();++i){
            cout<<file_occr[i].first<<endl;
        }
    }
    return 0;
}
