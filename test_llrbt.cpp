#include <iostream>
#include <cstdio>
#include <algorithm>
#include <vector>
#include <ctime>
#include <map>
#include <random>
#include <functional>

#include "llrbt.hpp"

int main() {
    std::default_random_engine rand_engine;
    std::uniform_int_distribution<int> rand_range(0, 1e9);
    auto rand_gen = std::bind(rand_range, rand_engine);
    std::vector<int>data;
    const int N = 1E6;
    for (int i = 0; i < N; ++i) {
        data.push_back(rand_gen());
    }
    std::sort(data.begin(),data.end());
    data.erase(std::unique(data.begin(), data.end()), data.end());
    for (int i = 0; i < data.size(); ++i) {
        int j = rand_gen() % data.size();
        std::swap(data[i], data[j]);
    }

    LLRBTree<int, int> rbt;
    std::map<int, int> mp;

    clock_t t1 = clock();
    for (int key: data) {
        rbt.Insert(key, 0);
    }
    clock_t t2 = clock();
    std::cout << "llrbt insert " << data.size() << " keys takes " << (t2 - t1) << " ms\n";

    t1 = clock();
    for (int key: data) {
        mp.insert({key, 0});
    }
    t2 = clock();
    std::cout << "map insert " << data.size() << " keys takes " << (t2 - t1) << " ms\n";

    for (int i = 0; i < data.size(); ++i) {
        int j = rand_gen() % data.size();
        std::swap(data[i], data[j]);
    }

    t1 = clock();
    for (int key: data) {
        rbt.Remove(key);
    }
    t2 = clock();
    std::cout << "llrbt remove " << data.size() << " keys takes " << (t2 - t1) << " ms\n";

    t1 = clock();
    for (int key: data) {
        mp.erase(key);
    }
    t2 = clock();
    std::cout << "map remove " << data.size() << " keys takes " << (t2 - t1) << " ms\n";
}
