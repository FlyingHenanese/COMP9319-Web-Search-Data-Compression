#include <iostream>
#include <list>
#include <queue>
#include <stack>
#include <math.h>
using namespace std;

class AdaptiveHuffmanTree
{
public:
    class Node
    {
    public:
        int weight;
        char alphabet;
        int implicit_number;
        Node* parent;
        Node* left_child;
        Node* right_child;
        Node(int we, char alp);
        ~Node();
    };
public:
    static const int alphabet_size = 256;
    int processed_alphabet;
    Node* root;
    Node* NYT;
    Node* find_Node(char seg);
    Node* add_new_element(char seg);
    Node* find_max_in_block(int weight);
    AdaptiveHuffmanTree();
    std::string encode(char seg);
    std::queue<AdaptiveHuffmanTree::Node*> find_block_list(int weight, bool search_type);
    void redefine_implicit_number();
    void update_tree(char seg);
    Node* slide_and_increment(AdaptiveHuffmanTree::Node *p);
    void print_tree();
    std::string decode(std::string input);
};

std::queue<AdaptiveHuffmanTree::Node*> AdaptiveHuffmanTree::find_block_list(int weight, bool search_type){
    //search type 1:q is leaf, search internal node with same weight
    //            0:q is internal node, search leaf node with weight+1
    std::queue<AdaptiveHuffmanTree::Node*> node_block_list;
    std::queue<AdaptiveHuffmanTree::Node*> reference_list;
    reference_list.push(root);
    while(!reference_list.empty()){
        AdaptiveHuffmanTree::Node* temp_node = reference_list.front();
        reference_list.pop();
        if(!search_type){
            if(temp_node->left_child==NULL) {
                if (temp_node->weight == weight+1) {
                    node_block_list.push(temp_node);
                }
            }
        }else{
            if(temp_node->left_child!=NULL){
                if(temp_node->weight==weight){
                    //if(temp_node!=root) {
                    node_block_list.push(temp_node);
                    //}
                }
            }
        }
        if(temp_node->right_child!=NULL){
            reference_list.push(temp_node->right_child);
        }
        if(temp_node->left_child!=NULL){
            reference_list.push(temp_node->left_child);
        }
    }
    return node_block_list;
}

AdaptiveHuffmanTree::Node* AdaptiveHuffmanTree::find_max_in_block(int weight) {
    std::queue<AdaptiveHuffmanTree::Node*> reference_list;
    reference_list.push(root);
    while(!reference_list.empty()) {
        AdaptiveHuffmanTree::Node* temp_node = reference_list.front();
        reference_list.pop();
        if(temp_node->left_child==NULL){
            if(temp_node->weight== weight){
                return temp_node;
            }
        }else{
            if(temp_node->right_child!=NULL){
                reference_list.push(temp_node->right_child);
            }
            if(temp_node->left_child){
                reference_list.push(temp_node->left_child);
            }
        }
    }
    return 0;
}

void AdaptiveHuffmanTree::redefine_implicit_number(){
    int current_implicit_number = 256;
    std::queue<Node*> all_node;
    std::queue<Node*> reference;
    reference.push(root);
    while(!reference.empty()){
        AdaptiveHuffmanTree::Node* temp = reference.front();
        reference.pop();
        all_node.push(temp);
        if(temp->right_child){
            reference.push(temp->right_child);
        }
        if(temp->left_child){
            reference.push(temp->left_child);
        }
    }
    while(!all_node.empty()){
        all_node.front()->implicit_number = current_implicit_number;
        all_node.pop();
        current_implicit_number= current_implicit_number-1;
    }

}

std::string AdaptiveHuffmanTree::encode(char seg) {
    std::string output = "";
    Node* node = find_Node(seg);
    if(!node){
        node=NYT;
        while(node != root){
            if(node==node->parent->left_child){output="0"+output;}
            else{output="1"+output;}
            node = node->parent;
        }
        std::bitset<8> binary(seg);
        output+=binary.to_string();
    }
    else{
        while(node != root){
            if(node==node->parent->left_child){output="0"+output;}
            else{output="1"+output;}
            node = node->parent;
        }
    }
    return output;
}

AdaptiveHuffmanTree::Node* AdaptiveHuffmanTree::find_Node(char seg) {
    std::queue<AdaptiveHuffmanTree::Node*> node_list;
    node_list.push(root);
    while(!node_list.empty()){
        AdaptiveHuffmanTree::Node* a = node_list.front();
        node_list.pop();
        if(a->alphabet==seg){
            return a;
        }
        if(a->left_child!=NULL){
            node_list.push(a->left_child);
        }
        if(a->right_child!=NULL){
            node_list.push(a->right_child);
        }
    }
    return NULL;
}

AdaptiveHuffmanTree::Node* AdaptiveHuffmanTree::add_new_element(char seg) {
    //split NYT and return old NYT
    Node* new_node = new Node(0,0);
    NYT->right_child = new_node;
    new_node->parent = NYT;
    //new_node->implicit_number = NYT->implicit_number-1;
    Node* new_lnode = new Node(0,0);
    NYT->left_child = new_lnode;
    //NYT->left_child->implicit_number=NYT->implicit_number-2;
    new_lnode->parent=NYT;
    NYT = new_lnode;
    new_node->alphabet = seg;
    return new_node->parent;
}

AdaptiveHuffmanTree::Node::Node(int we, char alp) {
    weight = we;
    alphabet = alp;
    implicit_number=0;
    left_child = NULL;
    right_child = NULL;
    parent = NULL;
}

//init tree
AdaptiveHuffmanTree::AdaptiveHuffmanTree() {
    processed_alphabet = 0;
    root = new Node(0, 0);
    NYT = root;
    root->implicit_number = 256;
}


void AdaptiveHuffmanTree::update_tree(char seg) {

    AdaptiveHuffmanTree::Node* leaf_to_increment=NULL;
    AdaptiveHuffmanTree::Node* q = find_Node(seg);
    if(!q){
        //begin special
        q = add_new_element(seg);
        AdaptiveHuffmanTree::redefine_implicit_number();
        leaf_to_increment = q->right_child;
        processed_alphabet=processed_alphabet+1;
    }
    else{
        AdaptiveHuffmanTree::Node* leader = AdaptiveHuffmanTree::find_max_in_block(q->weight);
        if(leader!=NULL){
            if(leader->parent!=q->parent){
                AdaptiveHuffmanTree::Node *temp_parent = leader->parent;
                leader->parent = q->parent;
                if (leader->parent->left_child == q) { leader->parent->left_child = leader; }
                else { leader->parent->right_child = leader; }
                q->parent = temp_parent;
                if (q->parent->left_child == leader) { q->parent->left_child = q; }
                else { q->parent->right_child = q; }
            } else{
                if(leader!= q){
                    AdaptiveHuffmanTree::Node* temp = q->parent->left_child;
                    q->parent->left_child = q->parent->right_child;
                    q->parent->right_child= temp;
                }
            }
            AdaptiveHuffmanTree::redefine_implicit_number();
        }
        if(q->parent->left_child == NYT){
            //special case 2
            leaf_to_increment = q;
            q = q->parent;
        }
    }
    while(q!=root){
        q=AdaptiveHuffmanTree::slide_and_increment(q);
    }
    q->weight=q->weight+1;
    if(leaf_to_increment){
        AdaptiveHuffmanTree::slide_and_increment(leaf_to_increment);
    }
}

AdaptiveHuffmanTree::Node* AdaptiveHuffmanTree::slide_and_increment(AdaptiveHuffmanTree::Node *p) {
    int wt = (p)->weight;
    AdaptiveHuffmanTree::Node* former_parent = (p)->parent;
    bool p_is_leaf;
    if((p)->left_child!=NULL){ p_is_leaf = false; }
    else{p_is_leaf = true;}
    std::queue<AdaptiveHuffmanTree::Node*> b;
    b = AdaptiveHuffmanTree::find_block_list(wt,p_is_leaf);
    std::stack<AdaptiveHuffmanTree::Node*> s;
    //if(b.front()==root){b.pop();}
    while (!b.empty()){
        s.push(b.front());
        b.pop();
    }
    if(!s.empty()){
        while(!s.empty()){
            AdaptiveHuffmanTree::Node* temp = s.top();
            s.pop();
            if((p)->parent!=temp->parent){
                AdaptiveHuffmanTree::Node* temp_parent = (p)->parent;
                (p)->parent = temp->parent;
                if((p)->parent->left_child == temp){(p)->parent->left_child = (p);}
                else{(p)->parent->right_child = (p);}
                temp->parent = temp_parent;
                if(temp->parent->left_child == (p)){temp->parent->left_child = temp;}
                else{temp->parent->right_child = temp;}
            }
            else{
                if((p)!=NYT && temp!=NYT){
                    if((p)!= temp){
                        AdaptiveHuffmanTree::Node* temp_node = (p);
                        (p)->parent->left_child = (p)->parent->right_child;
                        (p)->parent->right_child = temp_node;
                    }
                }
            }
            AdaptiveHuffmanTree::redefine_implicit_number();
        }
        (p)->weight = wt+1;
        if(p_is_leaf){

            p = (p)->parent;
        }
        else{
            p = former_parent;
        }
    }
    else{
        (p)->weight=wt+1;
        p = (p)->parent;
    }
    return p;
}

std::string AdaptiveHuffmanTree::decode(std::string input) {
    AdaptiveHuffmanTree::Node* current_node=root;
    std::string result = "";
    std::queue<char> input_stack;
    size_t j;
    for(j=0;input[j];j++){
        char temp= input[j];
        if(temp!=' '){
            input_stack.push(temp);
        }
    }
    while(!input_stack.empty()){
        std::string decode_binary="";
        int decode_dec=0;
        char decode_char=0;
        char path=0;
        cout<<"============"<<endl;
        if(current_node==NYT){
            for(int i=0; i<8; i++){
                char temp = input_stack.top();
                input_stack.pop();
                decode_binary+=temp;
            }
            size_t i;
            std::queue<char> temp;
            for(i=0;decode_binary[i]; ++i){
                temp.push(decode_binary[i]);
            }
            for(int i=0; i<8 ;++i){
                char c = temp.front();
                temp.pop();
                decode_dec+=(int(c)-48)*pow(2, 7-i);
            }
            decode_char = char(decode_dec);
            result+=decode_char;
            update_tree(decode_char);
            current_node = root;
        }else{
            path = input_stack.top();
            input_stack.pop();
            if(path == '1'){
                current_node = current_node->right_child;
            }
            if(path == '0'){
                current_node = current_node->left_child;
            }
            if(current_node->alphabet!=0){
                decode_char = current_node->alphabet;
                result+=decode_char;
                update_tree(decode_char);
                current_node = root;
            }
        }

    }
    return result;
}


void AdaptiveHuffmanTree::print_tree(){
    std::queue<Node*> test;
    std::queue<Node*> ref;
    ref.push(root);
    while(!ref.empty()){
        AdaptiveHuffmanTree::Node* temp = ref.front();
        ref.pop();
        test.push(temp);
        if(temp->right_child!=NULL){ref.push(temp->right_child);}
        if(temp->left_child!=NULL){ref.push(temp->left_child);}
    }
    while(!test.empty()){
        AdaptiveHuffmanTree::Node* temp = test.front();
        test.pop();
        cout<<temp->implicit_number<<"    "<<temp->alphabet<<"      "<<temp->weight<<endl;
    }
}


int main(int argc, char **argv) {
    while(1){
        AdaptiveHuffmanTree* ad = new AdaptiveHuffmanTree();
        string input;
        getline(cin,input);
        cout<<ad->decode(input)<<endl;
    }
}
