#include <iostream>
#include <vector>
#include <string>

int main(int argc, char* argv[])
{
    int sum = 0;
    int count = std::stoi(argv[1]);
    std::vector<int> v;
    v.reserve(count);
    for (int i = 1; i <= count; i++) {
        v.push_back(i);
    }

    for (int i = 0; i < 1000000; i++) {
        std::vector<int> v2 = v;
        for (const int& n : v2) {
            sum += n;
        }
    }
    std::cout << sum << "\n";
}
