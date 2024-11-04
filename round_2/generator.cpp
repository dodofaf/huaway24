#include <iostream>
#include <utility>
#include <vector>
#include <map>
#include <set>
#include <tuple>
#include <assert.h>
#include "algotester_generator.h"

using namespace std;

const int T = (int) 2e7;
const int MAX_CPU = 25600; // changed max cpu to 64 * 400
const int MAX_MEM = 524288; // changed max memory to 512 * 1024
const int MIN_DURATION = 21;
AlgotesterGenerator gen;

void get_vm_types(int m_set, vector<int> &cpu, vector<int> &mem, vector<double> &price) {
    if (m_set == 1) {
        cpu = {3164, 4760, 6356, 9548, 12740, 19124, 25508, 38276, 51044};
        mem = {59794, 90596, 121398, 185624, 249849, 378299, 506750, 763651, 1020552};
        price = {456, 684, 911, 1367, 1823, 2734, 3646, 5469, 7292};
    } else if (m_set == 2) {
        cpu = {
            376, 376, 772, 772, 772, 1568, 1568, 1568, 3164, 3164, 3164, 3164, 4760, 4760,
            6356, 6356, 6356, 6356, 9548, 9548, 9548, 11144, 12740, 12740, 12740, 19124, 19124,
            19124, 19124, 22316, 25508, 25508, 25508, 35084, 38276, 38276, 38276, 38276, 41468,
            51044, 51044, 51044
        };

        mem = {
            665, 1433, 1433, 3218, 7015, 3218, 7015, 14263, 7015, 14263, 29414, 59794, 44815,
            90596, 14263, 29414, 59794, 121398, 44815, 90596, 185624, 342173, 59794, 121398,
            249849, 90596, 185624, 378299, 956327, 691398, 121398, 249849, 506750, 169567,
            185624, 378299, 763651, 1919706, 1464108, 249849, 506750, 1020552
        };

        price = {
                86,    143,    229,    314,    486,    457,    629,    971,    943,   1300,
              2029,   4558,   5157,   6836,   1871,   2600,   6876,   9115,   6329,  10314,
             13673,  20471,   8429,  13753,  18230,  12657,  20629,  27344,  68113,  40943,
             16857,  27505,  36459,  28900,  32311,  41258,  54689, 136226,  94486,  43082,
             55011,  72918
        };
    }
    assert(cpu.size() == mem.size() && cpu.size() == price.size());
}

void generate_resource_req(int n, int *rc, int *rm) {
    double cpu_mean[2] = {0, 3.0};
    double cpu_sd[2] = {1.5, 1.3};

    for (int i = 1; i <= n; i++) {
        double cpu;
        if (gen.randInt(4) == 0) {
            cpu = 12.0;
        } else {
            int dist_group = gen.randInt(100) >= 75;
            int cpu_group = (int) (fabs(gen.randDoubleNormal(cpu_mean[dist_group], cpu_sd[dist_group]) - cpu_mean[dist_group]) + cpu_mean[dist_group]);
            if (cpu_group > 6)
                cpu_group = 6;
            
            if (cpu_group < 5) {
                int rnd = gen.randInt(10);
                int val = 1 << cpu_group;
                if (rnd < 7) {
                    cpu = val;
                } else if (rnd < 9) {
                    cpu = val * 1.25;
                } else {
                    vector<double> cand;
                    cand.push_back(val - 1);
                    for (double j = val + 0.25; j <= val + 1; j += 0.25)
                        cand.push_back(j);
                    for (double j = val + 1.5; j <= val + 2; j += 0.5)
                        cand.push_back(j);
                    for (double j = val + 3; j <= val + 4; j += 1.0)
                        cand.push_back(j);
                    for (int j = 0; j < cand.size(); j++) {
                        if (cand[j] == 0.0 || cand[j] > val * 1.5) {
                            swap(cand[j], cand[cand.size() - 1]);
                            cand.pop_back();
                            j--;
                        }
                    }
                    rnd = gen.randInt(cand.size());
                    cpu = cand[rnd];
                }
            } else if (cpu_group == 6) {
                vector<int> cand = {48, 64};
                cpu = cand[gen.randInt(cand.size())];
            } else { // cpu_group == 5
                if (gen.randInt(10)) {
                    cpu = 32;
                } else {
                    vector<int> cand = {25, 28, 30};
                    cpu = cand[gen.randInt(cand.size())];
                }
            }
        }
        
        int mem;
        int rnd = gen.randInt(3);
        if (!rnd) {
            mem = cpu * 2;
        } else if (rnd == 1 && cpu >= 2) {
            mem = cpu * 8 - gen.randInt(9);
        } else {
            vector<int> cand;
            int rnd = gen.randInt(cpu * 7) + 1;
            for (int step = 1; step <= cpu * 7; step *= 2) {
                if (rnd < step + step) {
                    for (int j = cpu + step; j <= cpu * 8; j += step) {
                        cand.push_back(j);
                    }
                    break;
                }
            }

            mem = cand[gen.randInt(cand.size())];
        }

        rc[i] = cpu * 400;
        rm[i] = mem * 1024;
        assert(rc[i] > 0 && rm[i] > 0 && rc[i] <= MAX_CPU && rm[i] <= MAX_MEM);
    }
}

double get_exp(double x, double w) {
    return log(1 - x) / (-w);
}

void generate_tenant(int tenant_id, int start, int n, int *rt, int *rd, int *rten) {
    int start_time = gen.randInt(0, 60 * 60 * 24);

    double diff_lambda = 0.001;

    double dur_mean[2] = {(double) gen.randDouble(100, 140), (double) gen.randDouble(210, 260)};
    double dur_sd[2] = {(double) gen.randDouble(15, 35), (double) gen.randDouble(20, 40)};
    double dur_lambda[2] = {0.0007, 0.00003};

    for (int i = start; i < start + n; i++) {
        int prev_time = i == start ? start_time : rt[i - 1];

        int diff;
        int rnd = gen.randInt(10);
        if (rnd < 5) {
            diff = fabs(gen.randDoubleNormal(0, 4));
        } else {
            diff = 10 + get_exp(gen.randDouble(), diff_lambda);
        }
        rt[i] = prev_time + diff;
        assert(rt[i] >= 0 && rt[i] < T && diff >= 0);
        
        int dur;
        do
        {
            rnd = gen.randInt(100);
	    rnd = 50;
            if (rnd < 10) {
                dur = dur_mean[1] + dur_sd[1] + get_exp(0, dur_lambda[0]) + get_exp(gen.randDouble(), dur_lambda[1]);
            } else if (rnd < 25) {
                dur = dur_mean[1] + dur_sd[1] + get_exp(gen.randDouble(), dur_lambda[0]);
            } else if (rnd < 70) {
                dur = gen.randDoubleNormal(dur_mean[1], dur_sd[1]) + 0.5;
            } else {
                dur = gen.randDoubleNormal(dur_mean[0], dur_sd[0]) + 0.5;
            }

            if (dur < 1)
                dur = gen.randInt(60) + 1;
        } while(dur < MIN_DURATION);

        rd[i] = dur;
        assert(rt[i] + rd[i] <= T);

        rten[i] = tenant_id;
    }
}

int main(int argc, char* argv[]) {
    gen = AlgotesterGenerator(argc, argv);

    // Processing arguments
    int n = gen.randInt(8000, 12000); // number of creation requests
    int tenants = gen.randInt(5, 15); // number of tenants
    int m_set = n >= 9000 ? 2 : 1; // VM types set. If m == 1 or m == 2 it will generate predefined vm types;
    int d = 40; // VMs creation delay

    
    // Generating VM types to be used
    vector<int> cpu, mem;
    vector<double> price;
    get_vm_types(m_set, cpu, mem, price);

    
    // Generating tenants sizes
    int rt[n + 5], rd[n + 5], rten[n + 5], rc[n + 5], rm[n + 5];
    memset(rm, -1, sizeof rm);
    vector<int> tenant_sizes(tenants, 0);
    int total = 0;
    for (int i = 0; i < tenants; i++) {
        int sz = i + 1 < tenants ? gen.randBiasedInt(n - total + 1, -(tenants - i - 1)) : n - total;
        if (sz == 0)
            sz = 1;
        tenant_sizes[i] = sz;
        total += sz;
    }

    assert(total == n);
    gen.shuffle(tenant_sizes);
    
    
    // Filling tenants
    total = 0;
    for (int i = 0; i < tenants; i++) {
        int sz = tenant_sizes[i];
        generate_tenant(i + 1, total + 1, sz, rt, rd, rten);
        total += sz;
    }
    assert(total == n);
    generate_resource_req(n, rc, rm);


    // Printing test
    int m = cpu.size();
    std::cout << m << ' ' << d << '\n';
    for (int i = 0; i < m; i++) {
        std::cout << cpu[i] << ' ' << mem[i] << ' ' << price[i] << '\n';
    }

    vector<int> order;
    for (int i = 1; i <= n; i++) {
        order.push_back(i);
    }

    sort(order.begin(), order.end(), [&](const int &i, const int &j) {
        return rt[i] < rt[j];
    });

    cout << n << '\n';
    for (int i = 0; i < n; i++) {
        int j = order[i];
        cout << rt[j] << ' ' << i + 1 << ' ' << rc[j] << ' ' << rm[j] << ' ' << rd[j] << '\n';
    }
}


