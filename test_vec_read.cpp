#include <iostream>
#include <vector>

int main(int argc, const char* argv[]) {
    int sum = 0;
    int count = std::stoi(argv[1]);
    std::vector<int> v;
    v.reserve(count);
    for (int i = 1; i <= count; i++) {
        v.push_back(i);
    }

    for (int i = 0; i < 1000000; i++) {
        for (const int& num : v) {
            sum += num;
        }
    }
    std::cout << sum << "\n";
}

