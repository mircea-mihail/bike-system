#ifndef MEMORY_POOL_LIST_H
#define MEMORY_POOL_LIST_H

#include "constants.h"
#include <iostream>

struct node{
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

    void push_freed(node *p_node)
    {
        if(m_freed_head == nullptr) 
        {
            p_node->m_next = nullptr;
            p_node->m_prev = nullptr;
            m_freed_head = p_node;
            return;
        }

        m_freed_head->m_next = p_node;
        p_node->m_prev = m_freed_head;
        p_node->m_next = nullptr;
        m_freed_head = p_node;
    }

    node *get_freed(node *p_prev_node, point p_pt, node *p_next_node)
    {
        if(m_freed_head == nullptr)
        {
            return new node(p_prev_node, p_pt, p_next_node);
        }

        if(m_freed_head->m_prev == nullptr)
        {
            node *return_node = m_freed_head;
            m_freed_head = nullptr;

            return_node->m_prev = p_prev_node;
            return_node->m_next = p_next_node;
            return_node->m_point = p_pt;
            
            return return_node;
        }

        m_freed_head->m_prev->m_next = nullptr;

        m_freed_head->m_prev = p_prev_node;
        m_freed_head->m_next = p_next_node;
        m_freed_head->m_point = p_pt;
        
        return m_freed_head;
    }

public:
    mp_list()
    {
        m_head = nullptr;
        m_tail = nullptr;

        m_freed_head = nullptr;
    }

    void push_front(point p_pt)
    {
        if(m_head == nullptr && m_tail == nullptr)
        {
            m_head = get_freed(nullptr, p_pt, nullptr);
            m_tail = m_head;
            return;
        }

        node *new_head = get_freed(m_head, p_pt, nullptr);
        m_head->m_next = new_head;

        m_head = new_head;
    }

    void push_back(point p_pt)
    {
        if(m_head == nullptr && m_tail == nullptr)
        {
            m_head = new node(nullptr, p_pt, nullptr);
            m_tail = m_head;
            return;
        }

        node *new_tail = new node(nullptr, p_pt, m_tail);
        m_tail->m_prev = new_tail;

        m_tail = new_tail;
    }

    void pop_back()
    {
        if(m_tail == nullptr)
        {
            return;
        }

        if(m_tail == m_head)
        {
            node *node_to_keep = m_tail;
            m_tail = nullptr;
            m_head = nullptr;

            push_freed(node_to_keep);
            return;
        }

        node *node_to_keep = m_tail;
        m_tail = m_tail->m_next;
        m_tail->m_prev = nullptr;
        push_freed(m_tail);
    }
    
    bool empty()
    {
        if(m_head == nullptr && m_tail == nullptr)
        {
            return true;
        }

        return false;
    }

    point back()
    {
        return m_tail->m_point;
    }

    void print_list()
    {
        node *n = m_head;
        while(n != nullptr)
        {
            std::cout << n->m_point.x << " " << n->m_point.y << std::endl;
        }
    }

};

#endif