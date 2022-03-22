#include <boost/unordered/unordered_map.hpp>
#include <boost/bimap.hpp>
#include <unordered_map>
#include <iostream>
#include <cxxabi.h>
#include <cstdlib>
#include <chrono>
#include <map>

class ClientInfo
{
public:
    ClientInfo() : countConnections(0) {}
    ClientInfo(int conecctions) : countConnections(conecctions) {}
    void incCon() { countConnections ++; }
    int countConnections;
};

template<typename Key, typename Value>
class MyMap
{
    using const_Key = const Key;
    using value_type = std::pair<const_Key, Value>;
public:
    class Node{
        public:
            value_type pair_obj;
            Node* left = nullptr;
            Node* right = nullptr;
            Node(const value_type &obj) : pair_obj(obj) {}
            int height = 0;
    };
    class Iterator 
    {
        public:
            Node *iter_node;
            Iterator(Node *n) : iter_node(n) {}
            value_type* operator->() { return &(iter_node->pair_obj); }
            bool operator !=(Iterator&& other) { return !(this->iter_node == other.iter_node); }
    };

private:
    Node *root = nullptr;

    void fix_tree_height(Node *head) {
        unsigned int left_height = head->left ? head->left->height : 0;
        unsigned int right_height = head->right ? head->right->height : 0;
        head->height = ((left_height > right_height) ? left_height : right_height) + 1;
    }

    void rotateright(Node** head) {
        Node* newHead;

        if(head && *head && (*head)->left) {
            newHead = (*head)->left;
            (*head)->left = newHead->right;
            newHead->right = *head;
            fix_tree_height(*head);
            fix_tree_height(newHead);
            *head = newHead;
        }
    }

    void rotateleft(Node** head) {
        Node* newHead;

        if(head && *head && (*head)->right) {
            newHead = (*head)->right;
            (*head)->right = newHead->left;
            newHead->left = *head;
                fix_tree_height(*head);
            fix_tree_height(newHead) ;
            *head = newHead;
        }
    }

    void balance(Node **head)
    {
        unsigned int left_height;
        unsigned int right_height;
        if(head && *head)
        {
            fix_tree_height(*head);
            left_height = (*head)->left ? (*head)->left->height : 0;
            right_height = (*head)->right ? (*head)->right->height : 0;
            if(right_height - left_height == 2) {
                left_height = (*head)->right->left ? (*head)->right->left->height : 0;
                right_height = (*head)->right->right ? (*head)->right->right->height : 0;
                if((int)right_height - (int)left_height < 0)
                    rotateright(&(*head)->right);
                rotateleft(head);
            } else if((int)right_height - (int)left_height == -2) {
                left_height = (*head)->left->left ? (*head)->left->left->height : 0;
                right_height = (*head)->left->right ? (*head)->left->right->height : 0;
                if((int)right_height - (int)left_height > 0)
                    rotateleft(&(*head)->left);
                rotateright(head);
            }
        }
    }

    Node *insert_tree(Node **head, Node *obj) {
        Node *result = nullptr;

        if(!*head) {
            *head = obj;
            obj->height = 1;
            result = *head;
        }
        else
        {
            if((*head)->pair_obj.first < obj->pair_obj.first) {
                result = insert_tree(&(*head)->right, obj);
                if(result == obj)
                    balance(head);
            } else if((*head)->pair_obj.first > obj->pair_obj.first) {
                result = insert_tree(&(*head)->left, obj);
                if(result == obj)
                    balance(head);
            } else /* equal */ {
                result = *head;
                delete obj;
            }
        }
        return result;
    }

    void delete_nodes(Node *n) {
        if(n) {
            delete_nodes(n->left);
            delete_nodes(n->right);
            delete n;
        }
    }

    Node *find_tree(Node *head, const_Key &obj_key) {
        if(head) {
            if(head->pair_obj.first == obj_key)
                return head;
            else if(head->pair_obj.first < obj_key) \
                return find_tree(head->right, obj_key);
            else
                return find_tree(head->left, obj_key);
        }
        return 0;
    }
public:
    Iterator insert(const value_type &obj) {
        return Iterator(insert_tree(&root, new Node(obj)));
    }
    Iterator find(const_Key &obj_key) {
        return Iterator(find_tree(root, obj_key));
    }
    void clear() {
        delete_nodes(root->left);
        delete_nodes(root->right);
        delete root;
        root = nullptr;
    }
    Iterator end() { return Iterator(nullptr); }
};

template<typename F, typename... Args>
double funcTime(F func, Args&&... args)
{
    auto t1 = std::chrono::high_resolution_clock::now();
    func(std::forward<Args>(args)...);
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - t1).count();
}

template<typename Container>
void insertMap(Container &a, unsigned int keys_range)
{
    for(decltype(keys_range) i = 0; i < keys_range; ++i)
        a.insert({i, 1});
}

template<typename Container>
void insertMapRandom(Container &a, unsigned int keys_range)
{
    for(decltype(keys_range) i = 0; i < keys_range; ++i)
        a.insert({std::rand(), 1});
}

template<typename Container>
void clientStatistics(Container &a, unsigned int keys_range, unsigned int clientConnections)
{
    for(decltype(clientConnections) i = 0; i < clientConnections; ++i) {
        auto key = std::rand() % keys_range;
        auto iterator = a.find(key);
        if(iterator != a.end()) {
            iterator->second.incCon(); //increase count of requests
        } else {
            a.insert({key, 1});
        }
    }
#if 0
    auto allCon = 0;
    for(auto iterator = a.begin(); iterator != a.end(); ++iterator) {
        if(allCon < 1000)
            std::cout << "For " << iterator->first << " present " << iterator->second.countConnections << " connections" << std::endl;
        allCon += iterator->second.countConnections;
    }
    if(allCon != clientConnections)
        std::cout << "Error " << allCon << " != " << clientConnections << std::endl;
#endif
}

template<typename Container>
void bench()
{
    const unsigned int keys_range = 1000000;
    const unsigned int connections = 10000000;
    Container a;

    auto realname = abi::__cxa_demangle(typeid(Container).name(), 0, 0, 0);
    std::cout << "Benchmark for " << realname << std::endl;
    free(realname);

    std::cout << "Insert " << keys_range << " values " << funcTime(insertMap<Container>, a, keys_range) << " us" << std::endl;
    a.clear();
    std::cout << "Insert " << keys_range << " random values " << funcTime(insertMapRandom<Container>, a, keys_range) << " us" << std::endl;
    a.clear();
    std::cout << "Simulation of " << keys_range << " users witt a total of " << connections << " connections " << funcTime(clientStatistics<Container>, a, keys_range, connections) << " us" << std::endl;
    a.clear();
}

int main()
{
    std::srand(std::time(nullptr));

    bench<std::map<long, ClientInfo>>();
    bench<std::unordered_map<long, ClientInfo>>();
    bench<boost::unordered_map<long, ClientInfo>>();
    bench<MyMap<long, ClientInfo>>();
    return 0;
}
