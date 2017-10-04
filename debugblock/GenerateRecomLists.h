#ifndef TEST_GENERATERECOMLISTS_H
#define TEST_GENERATERECOMLISTS_H

#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <stdio.h>


using namespace std;

typedef unordered_map<int, unordered_set<int>> CandSet;
typedef vector<vector<int>> Table;

double double_max(const double a, double b);

class GenerateRecomLists {
public:

    vector<vector<int>> generate_config(const vector<int>& field_list, const vector<int>& ltoken_sum_vector,
                              const vector<int>& rtoken_sum_vector, const double field_remove_ratio,
                              const uint32_t ltable_size, const uint32_t rtable_size);

    GenerateRecomLists();
    ~GenerateRecomLists();
};


#endif //TEST_GENERATERECOMLISTS_H
