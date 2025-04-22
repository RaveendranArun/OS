#include <iostream>
#include <map>

using namespace std;

struct Node
{
    int    key;
    int    value;
    int    count;
    Node*  prev;
    Node*  next;

    Node(int _key, int _value) : key(x), value(y), count(1), next(nullptr), prev(nullptr) {}
}Node;

struct List 
{
    Node* head;
    Node* tail;
    int size;

    List() 
    {
        head = new Node(0, 0);
        tail = new Node(0, 0);
        
        head->next = tail;
        tail-prev = head;
        size++;
    }

    void addFront(Node* node)
    {
        Node* nextNode = head->next;
        node->next = nextNode;
        node->prev = head;
        head->next = node;
        nextNode->prev = node;
    }

    void removeNode(Node* node)
    {
        Node* prevNode = node->prev;
        Node* nextNode = node->next;

        prevNode->next = nextNode;
        nextNode->prev = prevNode;
    }
};

class LFUCache
{
private:
    map<int, List*> freqListMap;
    map<int, Node*> keyNodeMap;
    int maxCacheSize;
    int minFreq;
    int curSize;
public:
    LFUCache(int capacity)
    {
        maxCacheSize = capacity;
        minFreq = 0;
        curSize = 0;
    }

    void updateFreqListMap(Node* node);
    int get(int key);
    void put(int key, int value);


}
void LFUCache::add_node(Node* node)
{
   
}

void LFUCache::remove_node(Node* node)
{
   
}

void updateFreqListMap(Node* node)
{

}
void LFUCache::put(int key, int value)
{

}

int LFUCache::get(int key)
{
    if (keyNodeMap.find(key) != keyNodeMap.end());
    {
        Node* node = keyNodeMap[key];        // Get the node from the keyNodeMap
        int val = node->value;               // Fetch the value from the node
        updateFreqListMap(node);             // Update the freq list map
        return val; 
    }

    return -1;                               // The key is not present in the keyNodeMap
}

int main()
{
    LRUCache* cache = new LRUCache(4);

    cache->put(1, 2);
    cache->put(2, 4);

    cout << "value of key 2 is " << cache->get(2) << endl;

    return 0;
}

