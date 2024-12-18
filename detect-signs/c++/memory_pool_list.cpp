// -------------------------- private functions
#include "memory_pool_list.h"

void mp_list::push_freed(node *p_node)
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

node *mp_list::get_freed(node *p_prev_node, point p_pt, node *p_next_node)
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

    node *ret_node = m_freed_head;
    m_freed_head = m_freed_head->m_prev;
    m_freed_head->m_next = nullptr;

    ret_node->m_prev = p_prev_node;
    ret_node->m_next = p_next_node;
    ret_node->m_point = p_pt;
    
    return ret_node;
}

// ------------------------ private functions

mp_list::mp_list()
{
    m_head = nullptr;
    m_tail = nullptr;

    m_freed_head = nullptr;
}

void mp_list::push_front(point p_pt)
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

void mp_list::push_back(point p_pt)
{
    if(m_head == nullptr && m_tail == nullptr)
    {
        m_head = get_freed(nullptr, p_pt, nullptr);
        m_tail = m_head;
        return;
    }

    node *new_tail = get_freed(nullptr, p_pt, m_tail);
    m_tail->m_prev = new_tail;

    m_tail = new_tail;
}

void mp_list::pop_back()
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

    node *node_to_push = m_tail;
    m_tail = m_tail->m_next;
    m_tail->m_prev = nullptr;
    push_freed(node_to_push);
}

bool mp_list::empty()
{
    if(m_head == nullptr && m_tail == nullptr)
    {
        return true;
    }

    return false;
}

point mp_list::back()
{
    return m_tail->m_point;
}

void mp_list::print_list()
{
    std::cout << "current list:"<< std::endl;
    node *n = m_head;
    while(n != nullptr)
    {
        std::cout << n->m_point.x << " " << n->m_point.y << std::endl;
        n = n->m_prev;
    }
}

mp_list::~mp_list()
{
    node *n = m_head;
    while(n != nullptr)
    {
        node *next_node = n->m_prev;
        delete n;

        n = next_node;
    }

    n = m_freed_head;
    while(n != nullptr)
    {
        node *next_node = n->m_prev;
        delete n;

        n = next_node;
    }
}