#include <bits/stdc++.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
using namespace std;


int main(int argc, char *argv[]) {

// Read from argument and get values for all PEs for one PAPI event. (found in pp/)
    int opt;
    char *papi_event;
    while ((opt = getopt(argc, argv, "P:")) != -1) {
        switch (opt) {
            case 'P': papi_event = optarg; break;
            default:  break;
        }
    }

    vector<long long> values;
    string file_dir = "./pp/";
    file_dir += papi_event;
    ifstream fp(file_dir, ios::in);
    string buf;
    while(getline(fp, buf)) {
        long long num = 0;
        for(auto c : buf) {
            num += num*10 + (c - '0');
        }
        values.push_back(num);
    }
    long long avg = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
    FILE *fp2 = fopen("exp2_result.txt", "a+");
    fprintf(fp2, "%lld\n", avg);
}