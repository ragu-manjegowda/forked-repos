#include <iostream>
#include <vector>

int main(int argc, const char* argv[]) {
    int sum = 0;
    std::vector<int> v = std::vector<int>({ 1, 2, 3});

    for (int i = 0; i < 1000000; i++) {
        for (const int& num : v) {
            sum += num;
        }
    }
    std::cout << sum << "\n";
}

