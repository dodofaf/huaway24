#include <iostream>
#include <utility>
#include <vector>
#include <map>
#include <set>
// #include "algotester.h"
// #include "algotester_generator.h"
using namespace std;

#define FOR(i,a,b) for (int i = (a); i < (b); i++)
#define RFOR(i,b,a) for (int i = (b) - 1; i >= (a); i--)
#define ITER(it,a) for (__typeof(a.begin()) it = a.begin(); it != a.end(); it++)
#define FILL(a,value) memset(a, value, sizeof(a))

#define SZ(a) (int)a.size()
#define ALL(a) a.begin(), a.end()
#define PB push_back
#define MP make_pair

typedef long long LL;
typedef vector<int> VI;
typedef pair<LL, LL> PII;

//const double PI = acos(-1.0);
const int INF = 1000 * 1000 * 1000 + 7;
const LL LINF = INF * (LL) INF;

struct VMType
{
    int cpu;
    int mem;
    int price;

    VMType(int cpu, int mem, int price)
    {
        this->cpu = cpu;
        this->mem = mem;
        this->price = price;
    }
};

vector<VMType> vm_types;

int find_vm_type(int cpu, int mem)
{
    int best_price = INF;
    int ind = -1;
    FOR (i, 0, SZ(vm_types))
    {
        if (vm_types[i].cpu >= cpu && vm_types[i].mem >= mem && vm_types[i].price < best_price)
        {
            best_price = vm_types[i].price;
            ind = i;
        }
    }

    return ind;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false); // cin.tie(0);

    int m, d;
    cin >> m >> d;

    FOR (i, 0, m)
    {
        int cpu, mem, price;
        cin >> cpu >> mem >> price;

        vm_types.PB(VMType(cpu, mem, price));
    }

    VI vms_to_shutdown;
    map<int, VI> containers_to_allocate;

    int t;
    cin >> t;
    FOR (j, 0, t)
    {
        int e;
        cin >> e;
        if (e == -1) return 0; // :(

        int cnt = 1;
        if (e == 0) cin >> cnt;

        VI containers_to_shutdown;
        vector<PII> vms_to_create;
        FOR (k, 0, e)
        {
            int type;
            cin >> type;
            if (type == 1)
            {
                int id, cpu, mem;
                cin >> id >> cpu >> mem;

                vms_to_create.PB(MP(id, find_vm_type(cpu, mem)));
                containers_to_allocate[j + d].PB(id);
            }
            if (type == 2)
            {
                int id;
                cin >> id;

                containers_to_shutdown.PB(id);
            }
        }

        j--;
        FOR (it, 0, cnt)
        {
            j++;

            cout << SZ(vms_to_create) + SZ(vms_to_shutdown) + (containers_to_allocate.count(j) > 0 ? SZ(containers_to_allocate[j]) : 0) << "\n";

            FOR (k, 0, SZ(vms_to_create))
            {
                cout << "1 " << vms_to_create[k].first << ' ' << vms_to_create[k].second + 1 << "\n";
            }
            FOR (k, 0, SZ(vms_to_shutdown))
            {
                cout << "2 " << vms_to_shutdown[k] << "\n";
            }

            if (containers_to_allocate.count(j))
            {
                auto& v = containers_to_allocate[j];
                FOR (k, 0, SZ(v))
                {
                    cout << "3 " << v[k] << ' ' << v[k] << "\n";
                }
            }

            vms_to_shutdown = containers_to_shutdown;
            containers_to_shutdown.clear();
            vms_to_create.clear();

        }
        cout.flush();
    }

    int e;
    cin >> e;
    if (e == 0)
    {
        // :)
    }
    else
    {
        // :(
    }

}



