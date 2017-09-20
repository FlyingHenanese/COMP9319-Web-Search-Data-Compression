#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <stdlib.h>
#include <string>
#include <list>

using namespace std;

const int length = 1000;
ifstream outfile;
ifstream idx_read;
char *search_option;
char *bwt_file;
char *idx_file;
char *search_pattern;
vector<int> generate_idx_file();
vector<int> generate_idx_file_short();
int first(vector<int>, int);
int occurance_function(char,int);
int occurance_function_short(char, int);
vector<int> back_ward_search(const char *, vector<int>);
vector<int> back_ward_search_short(const char*, vector<int>);
list<unsigned int> get_idx_number(vector<int>, vector<int>);
list<unsigned int> get_idx_number_short(vector<int>, vector<int>);

list<unsigned int> get_idx_number(vector<int> idx_list, vector<int> FL_list){
    int First = FL_list[0]-1;
    outfile.clear();
    outfile.seekg(0, ios::beg);
    list<unsigned int> result;
    while(First<=FL_list[1]-1){
        int current_idx = First;
        char current = outfile.seekg(current_idx).get();
        string idx_number="";
        bool repeat = false;
        while(int(current)!=93){
            outfile.clear();
            outfile.seekg(0, ios::beg);
            current_idx = first(idx_list, int(current))+occurance_function(current, current_idx);
            current = outfile.seekg(current_idx).get();
        }
        current_idx = first(idx_list, int(current))+occurance_function(current, current_idx);
        current = outfile.seekg(current_idx).get();
        while(int(current)!=91){
            outfile.clear();
            outfile.seekg(0, ios::beg);
            idx_number=current+idx_number;
            current_idx = first(idx_list, int(current))+occurance_function(current, current_idx);
            current = outfile.seekg(current_idx).get();
        }
        unsigned int idx_int = 0;
        idx_int = stoul(idx_number, nullptr,0);
        result.push_back(idx_int);
        First+=1;
    }
    result.sort();
    result.unique();
    return result;
}



vector<int> generate_idx_file(){
    vector<int> index_list(128);
    outfile.clear();
    outfile.seekg(0, ios::beg);
    ifstream test(idx_file);
    if(!test.good()){
        if(outfile.is_open()){
            char ele;
            int size = 0;
            ofstream idx_write;
            idx_write.open(idx_file,ios::binary);
            while(!outfile.eof()){
                while(size<length){
                    ele = outfile.get();
                    int asc_value = int(ele);
                    if(asc_value<128 && asc_value>=0){
                        index_list[asc_value]+=1;
                    }
                    size+=1;
                }
                size=0;
                for (int i=0; i<128; ++i) {
                    idx_write.write((char*)&index_list[i], sizeof(int));
                }
            }                
            idx_write.close();
        }
    }
    else{
        if(outfile.is_open()){
            char ele;
            int size = 0;
            while(!outfile.eof()){
                while(size<length){
                    ele = outfile.get();
                    int asc_value = int(ele);
                    if(asc_value<128 && asc_value>=0){
                        index_list[asc_value]+=1;
                    }
                    size+=1;
                }
                size=0;
            }
        }
    }
    return index_list;
}

vector<int> back_ward_search(const char* pattern, vector<int> index_list) {
    vector<int> FL(2);
    string P = string(pattern);
    int i = P.length();
    char c = P[i - 1];
    int First = 0;
    int Last = 0;
    int asc_num = int(c);
    int sum_of_previous = 0;
    sum_of_previous = first(index_list, asc_num);
    First = sum_of_previous + 1;
    Last = sum_of_previous + index_list[asc_num];
    while ((First <= Last) and (i >= 2)) {
        c = P[i - 2];
        First = first(index_list, int(c)) + occurance_function(c, First - 1) + 1;
        Last = first(index_list, int(c)) + occurance_function(c, Last);
        i -= 1;
        
    }
    if(Last<First){
        FL[0] = 1;
        FL[1] = 0;
        return FL;
    }
    else{
        FL[0] = First;
        FL[1] = Last;
        return FL;
    }
}

int first(vector<int> index_list,int asc_num){
    int position=0;
    int sum_of_previous=0;
    for(position = 0; position<asc_num;++position){
        if(index_list[position]!=0){
            sum_of_previous+= index_list[position];
        }
    }
    return sum_of_previous;
}

int occurance_function(char c, int position){
    
    int num_of_bucket=0;
    int times_of_appearance= 0;
    if(position>length) {
        num_of_bucket = position / length - 1;
        idx_read.clear();
        idx_read.seekg(0,ios::beg);
        int idx_position = 4*(num_of_bucket*128);
        clock_t t;
        t = clock();
        idx_read.seekg(idx_position+int(c)*4);
        idx_read.read((char*)&times_of_appearance,sizeof(times_of_appearance));
        t =  clock()-t;
        double time_taken = ((double)t)/CLOCKS_PER_SEC;
        printf("%f\n", time_taken);
    }else{
        times_of_appearance = 0;
    }
    if(outfile.is_open()){
        outfile.clear();
        outfile.seekg(0, ios::beg);
        outfile.seekg(position-(position%length));
        int temp = position-(position%length);
        while(temp<position){
            char ele = outfile.get();
            if(ele == c){
                times_of_appearance+=1;
            }
            temp+=1;
        }
    }
    
    return times_of_appearance;

}

list<unsigned int> get_idx_number_short(vector<int> idx_list, vector<int> FL_list){
    int First = FL_list[0]-1;
    outfile.clear();
    outfile.seekg(0, ios::beg);
    list<unsigned int> result;
    while(First<=FL_list[1]-1){
        int current_idx = First;
        char current = outfile.seekg(current_idx).get();
        string idx_number="";
        bool repeat = false;
        while(int(current)!=93){
            outfile.clear();
            outfile.seekg(0, ios::beg);
            current_idx = first(idx_list, int(current))+occurance_function_short(current, current_idx);
            current = outfile.seekg(current_idx).get();
        }
        current_idx = first(idx_list, int(current))+occurance_function_short(current, current_idx);
        current = outfile.seekg(current_idx).get();
        while(int(current)!=91){
            outfile.clear();
            outfile.seekg(0, ios::beg);
            idx_number=current+idx_number;
            current_idx = first(idx_list, int(current))+occurance_function_short(current, current_idx);
            current = outfile.seekg(current_idx).get();
        }
        unsigned int idx_int = 0;
        idx_int = stoul(idx_number, nullptr,0);
        result.push_back(idx_int);
        First+=1;
    }
    result.sort();
    result.unique();
    return result;
}

vector<int> back_ward_search_short(const char* pattern, vector<int> index_list) {
    vector<int> FL(2);
    string P = string(pattern);
    int i = P.length();
    char c = P[i - 1];
    int First = 0;
    int Last = 0;
    int asc_num = int(c);
    int sum_of_previous = 0;
    sum_of_previous = first(index_list, asc_num);
    First = sum_of_previous + 1;
    Last = sum_of_previous + index_list[asc_num];
    while ((First <= Last) and (i >= 2)) {
        c = P[i - 2];
        First = first(index_list, int(c)) + occurance_function_short(c, First - 1) + 1;
        Last = first(index_list, int(c)) + occurance_function_short(c, Last);
        i -= 1;        
    }
    if(Last<First){
        FL[0] = 1;
        FL[1] = 0;
        return FL;
    }
    else{
        FL[0] = First;
        FL[1] = Last;
        return FL;
    }
}

int occurance_function_short(char c, int position){
    int temp=0;
    int times_of_appearance = 0;
    if(outfile.is_open()){
        outfile.clear();
        outfile.seekg(0, ios::beg);
        while(temp<position){
            char ele = outfile.get();
            if(ele == c){
                times_of_appearance+=1;
            }
            temp+=1;
        }
    }
    return times_of_appearance;

}

vector<int> generate_idx_file_short(){
    vector<int> index_list(128);
    outfile.clear();
    outfile.seekg(0, ios::beg);
    if(outfile.is_open()){
        char ele;
        int size = 0;
        while(!outfile.eof()){
            ele = outfile.get();
            int asc_value = int(ele);
            if(asc_value<128 && asc_value>=0){
                index_list[asc_value]+=1;
            }
        }
    }
    return index_list;
}



int main(int argc, char **argv)  {
    if(argc==5){
        search_option = argv[1];
        bwt_file = argv[2];
        idx_file = argv[3];
        search_pattern = argv[4];
        outfile.open(bwt_file,ios::in);
        outfile.seekg(0,ios::end);
        streampos ps = outfile.tellg();
        if(ps>1000){            
            vector<int> idx= generate_idx_file();
            idx_read.open(idx_file,ios::binary);
            vector<int> search_result = back_ward_search(search_pattern, idx);
            if(string(search_option)=="-n") {
                cout<<search_result[1]-search_result[0]+1<<endl;
            }
            else if(string(search_option)=="-a"){
                list<unsigned int> result = get_idx_number(idx, search_result);
                while(!result.empty()){
                    cout<<"["<<result.front()<<"]"<<endl;
                    result.pop_front();
                }
            }
            else if(string(search_option)=="-r"){
                list<unsigned int> result = get_idx_number(idx, search_result);
                cout<<result.size()<<endl;
            }
            idx_read.close();
        }
        else{
            vector<int> idx= generate_idx_file_short();
            vector<int> search_result = back_ward_search_short(search_pattern, idx);
            if(string(search_option)=="-n") {
                cout<<search_result[1]-search_result[0]+1<<endl;
            }
            else if(string(search_option)=="-a"){
                list<unsigned int> result = get_idx_number_short(idx, search_result);
                while(!result.empty()){
                    cout<<"["<<result.front()<<"]"<<endl;
                    result.pop_front();
                }
            }
            else if(string(search_option)=="-r"){
                list<unsigned int> result = get_idx_number_short(idx, search_result);
                cout<<result.size()<<endl;
            }
        }
    } else {
        cout<<"Usage: bwtsearch -option *.bwt *.idx search_pattern"<<endl;
    }
    outfile.close();

    return 0;
}


