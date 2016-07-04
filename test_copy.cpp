#include <iostream>
#include <vector>

int main(int argc, char* argv[])
{
    int sum = 0;
    std::vector<int> v = { 1, 2, 3 };

    for (int i = 0; i < 1000000; i++) {
        std::vector<int> v2 = v;
        for (const int& n : v2) {
            sum += n;
        }
    }
    std::cout << sum << "\n";
}
