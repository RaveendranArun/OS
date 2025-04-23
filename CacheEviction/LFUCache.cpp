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

    Node(int _key, int _value) : key(_key), value(_value), count(1), next(nullptr), prev(nullptr) {}
};

struct List 
{
    Node* head;
    Node* tail;
    int   size;

    List() 
    {
        head = new Node(0, 0);
        tail = new Node(0, 0);
        
        head->next = tail;
        tail->prev = head;
        size = 0;
    }

    void addNodeFront(Node* node)
    {
        Node* nextNode = head->next;
        node->next = nextNode;
        node->prev = head;
        head->next = node;
        nextNode->prev = node;
        size++;
    }

    void removeNode(Node* node)
    {
        Node* prevNode = node->prev;
        Node* nextNode = node->next;

        prevNode->next = nextNode;
        nextNode->prev = prevNode;
        size--;
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


};

void LFUCache::updateFreqListMap(Node* node)
{
    keyNodeMap.erase(node->key);
    freqListMap[node->count]->removeNode(node);


}
void LFUCache::put(int key, int value)
{
    if (maxCacheSize == 0)                           // The cache is inialized with capacity zero
    {
        return;
    }

    if (keyNodeMap.find(key) != keyNodeMap.end())    // If the key is already present
    {
        Node* node = keyNodeMap[key];
        node->value = value;
        updateFreqListMap(node);
    }
    else
    {
        if (curSize == maxCacheSize)                 // Cache is full, eviction based on LRU
        {
            List* list = freqListMap[minFreq];
            keyNodeMap.erase(list->tail->prev->key);
            freqListMap[minFreq]->removeNode(list->tail->prev);
            curSize--;
        }

        curSize++;                                  // Update the new (key, value) pair
        minFreq = 1;
        List* listFreq;
        if (freqListMap.find(minFreq) != freqListMap.end())
        {
            listFreq = freqListMap[minFreq];
        }
        else
        {
            listFreq = new List();
            freqListMap[minFreq] = listFreq;
        }
        
        Node* node = new Node(key, value);
        listFreq->addNodeFront(node);
        keyNodeMap[key] = node;
    }
    
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

    return 0;
}

