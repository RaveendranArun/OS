#include <iostream>
#include <map>

using namespace std;

class Node
{
public:
    int    key;
    int    value;
    Node*  prev;
    Node*  next;

    Node() : key(-1), value(-1), next(nullptr), prev(nullptr) {}
    Node(int x, int y) : key(x), value(y), next(nullptr), prev(nullptr) {}
};

class LRUCache
{
private:
    Node* m_head = new Node();
    Node* m_tail = new Node();
    int   m_capacity;
    map<int, Node*> m_mpp;

public:
    LRUCache(int capacity)
    {
        m_capacity = capacity;
        m_head->next = m_tail;
        m_tail->prev = m_head;
    }

    void add_node(Node* node);
    void remove_node(Node* node);
    void put(int key, int value);
    int get(int key);
};

void LRUCache::add_node(Node* node)
{
    Node* nextNode = m_head->next;

    node->next = nextNode;
    nextNode->prev = node;
    m_head->next = node;
    node->prev = m_head;
}

void LRUCache::remove_node(Node* node)
{
    Node* prevNode = node->prev;
    Node* nextNode = node->next;


    prevNode->next = nextNode;
    nextNode->prev = prevNode;
}

void LRUCache::put(int key, int value)
{
    // 1. Does the (key, value) already exists or not
    if (m_mpp.find(key) != m_mpp.end())
    {
        Node* node = m_mpp[key];         // Retrieve the node from th map
        m_mpp.erase(key);                // Erase the entry from the map
        remove_node(node);             // Remove the node
    }

    if (m_mpp.size() == m_capacity)
    {
        Node* node = m_tail->prev;       // Get the last node from the tail
        m_mpp.erase(node->key);        // Remove the entry form the map
        remove_node(node);             // Remove the node
    }

    add_node(new Node(key, value));
    m_mpp[key] = m_head->next;    
}

int LRUCache::get(int key)
{
    if (m_mpp.find(key) != m_mpp.end())
    {
        Node* node = m_mpp[key];
        int res = node->value;
        m_mpp.erase(key);
        remove_node(node);
        add_node(node);
        m_mpp[key] = node;

        return res;
    }

    return -1;
}

int main()
{
    LRUCache* cache = new LRUCache(4);

    cache->put(1, 2);
    cache->put(2, 4);

    cout << "value of key 2 is " << cache->get(2) << endl;

    return 0;
}
