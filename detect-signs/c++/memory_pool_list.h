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

    void push_freed(node *p_node);

    node *get_freed(node *p_prev_node, point p_pt, node *p_next_node);

public:
    mp_list();

    void push_front(point p_pt);

    void push_back(point p_pt);

    void pop_back();
       
    bool empty();

    point back();

    void print_list();

    ~mp_list();
};

#endif