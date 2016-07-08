
#include "cow.h"
#include <iostream>
#include <string>

int main(int argc, const char* argv[]) {
    int count = std::stoi(argv[1]);
    std::vector<int> tmp;
    tmp.reserve(count);
    for (int i = 1; i <= count; i++) {
        tmp.push_back(i);
    }
    cow_vector<int> v(tmp);
    for (int i = 0; i < 1000000; i++) {
        v.erase_at(count-1);
        v.push_back(count);
    }
    return 0;
}

