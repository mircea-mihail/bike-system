#ifndef MEMORY_POOL_LIST_H
#define MEMORY_POOL_LIST_H

#include "constants.h"
#include <iostream>

struct node
{
    node *m_prev;
    point m_point;
    node *m_next;

    node()
    {
        m_prev = nullptr;
        m_next = nullptr;
        m_point = point();
    }

    node(node *p_prev, point p_pt, node *p_next): m_prev(p_prev), m_point(p_pt), m_next(p_next){ }
};

class mp_list
{
private:
    node *m_head;
    node *m_tail;

    node *m_freed_head;

    /// @brief detaches a freed node from the main list and adds it to the hidden freed list
    /// in order to avoid de-alocation
    /// @param p_node the node to add to the freed list
    void push_freed(node *p_node);

    /// @brief unlinks a node from the freed list and adds it to the main list
    /// if freed list is empty, it allocates a new node and returns it 
    /// @param p_prev_node the node to have before the returned node
    /// @param p_pt the value of the returned node
    /// @param p_next_node the node to have after the returned node
    /// @return 
    node *get_freed(node *p_prev_node, point p_pt, node *p_next_node);

public:
    /// @brief initialises a null head tail and freed_head
    mp_list();

    /// @brief adds a new node to the front of the list using the get_freed function
    /// @param p_pt the value of the new node
    void push_front(point p_pt);

    /// @brief adds a new node to the back of the list using the get_freed function
    /// @param p_pt the value of the new node
    void push_back(point p_pt);

    /// @brief deletes a node from the back of the list
    void pop_back();
       
    /// @brief checks if the list is empty
    /// @return true if it is empty, false if not
    bool empty();

    /// @brief gets the value at the back of the list 
    /// @return returns this value
    point back();

    /// @brief used for debugging, prints every element in the list from front to back
    void print_list();

    /// @brief de-alocates all the nodes in the main list and in the freed list
    ~mp_list();
};

#endif