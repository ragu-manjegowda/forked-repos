#include <iostream>
#include <vector>
#include <string>

int main(int argc, const char* argv[]) {
    int count = std::stoi(argv[1]);
    std::vector<int> v;
    v.reserve(count);
    for (int i = 1; i <= count; i++) {
        v.push_back(i);
    }

    for (int i = 0; i < 1000000; i++) {
        v.erase(v.begin() + count - 1);
        v.push_back(count);
    }
}

