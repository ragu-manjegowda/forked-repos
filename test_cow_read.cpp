#include "cow.h"
#include <iostream>
#include <string>

int main(int argc, const char* argv[]) {
    int sum = 0;
    int count = std::stoi(argv[1]);
    std::vector<int> tmp;
    tmp.reserve(count);
    for (int i = 1; i <= count; i++) {
        tmp.push_back(i);
    }
    cow_vector<int> v(tmp);

    for (int i = 0; i < 1000000; i++) {
        cow_vector_read<int> v2 = v.for_reading();
        for (const int& num : *v2) {
            sum += num;
        }
    }
    std::cout << sum << "\n";
    return 0;
}
