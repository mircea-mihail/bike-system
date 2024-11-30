#include <iostream>
#include "memory_pool_list.h"

int main()
{
    int v = -1;
    mp_list mp;
    mp.push_back(point(v, v)); v++;
    mp.push_back(point(v, v)); v++;
    mp.push_back(point(v, v)); v++;
    mp.push_back(point(v, v)); v++;
    mp.push_front(point(v, v)); v++;
    mp.push_front(point(v, v)); v++;
    mp.push_front(point(v, v)); v++;

    mp.print_list();

}