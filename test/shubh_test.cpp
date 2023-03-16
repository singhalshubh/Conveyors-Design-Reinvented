#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
using namespace std;

int main() {

    vector<string>papi_counters;
    map<string, long long> m;
    std::ifstream fp("papi_counter_shubh.txt", std::ios::binary);
    string buf;
    while(getline(fp, buf)) {
        papi_counters.push_back(buf);
    }
    vector<long long> vec1;
    for(auto c: papi_counters) {
        string s = "./pp/" + c;
        std::ifstream fp1(s, std::ios::binary);
        
        string count;
        std::vector<long long> vec;
        while(getline(fp1, count)) {
            vec.push_back(stol(count.c_str(), nullptr, 10));
        }
        long long avg = std::accumulate(vec.begin(), vec.end(), 0.0) / vec.size();
        vec1.push_back(avg);
    }

    FILE *fp2 = fopen("shubh_result.txt", "+a");
    for(auto c : vec1) {
        fprintf(fp2, "%lld\n", c);
    }
    fclose(fp2);
}