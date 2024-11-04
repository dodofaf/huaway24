#include <iostream>
#include <sstream>
#include <utility>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <string>
#include <math.h>
#include <algorithm>
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

struct VM {
    int type;
    int free_cpu;
    int free_mem;
    int ind;
    int start_time;

    VM() {}
    VM(int type_ind, int start)
    {
        this->type = type_ind;
        this->free_cpu = vm_types[type_ind].cpu;
        this->free_mem = vm_types[type_ind].mem;
        this->start_time = start;
    }
};

map<int, VM> vms;

struct Container
{
    int cpu;
    int mem;
    int vm_ind;

    Container() {}
    Container(int cpu, int mem)
    {
        this->cpu = cpu;
        this->mem = mem;
        this->vm_ind = -1;
    }
};

map<int, Container> containers;

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

int find_vm(int cpu, int mem)
{
    auto best_itr = vms.end();
    for (auto itr=vms.begin();itr!=vms.begin();++itr) {
        if (itr->second.free_cpu >= cpu && itr->second.free_mem >= mem) {
            if (best_itr == vms.end() || (itr->second.free_cpu >= best_itr->second.free_cpu && itr->second.free_mem >= best_itr->second.free_mem))
                best_itr = itr;
        }
    }
    return best_itr->second.ind;
}

void clear_resources(int cont_ind)
{
    auto cont = containers.find(cont_ind)->second;
    auto vm = vms.find(cont.vm_ind);
    vm->second.free_cpu += cont.cpu;
    vm->second.free_mem += cont.mem;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);


    int m, d;
    cin >> m >> d;

    FOR (i, 0, m)
    {
        int cpu, mem, price;
        cin >> cpu >> mem >> price;

        vm_types.PB(VMType(cpu, mem, price));
    }

//    list<pair<int, int>> unavailible_vms;
    list<int> new_containers;
    list<int> shutdown_containers;
    map<int, list<int>> containers_to_allocate;

    int cnt_vms = 1;
    stringstream output;
    int cnt_out = 0;

    int t;
    cin >> t;
    FOR (j, 0, t)
    {
        int e;
        cin >> e;
        if (e == -1) return 0; //

        int cnt = 1;
        if (e == 0) cin >> cnt;

//        list<int> freed_vms;
        FOR (k, 0, e)
        {
            int type;
            cin >> type;
            if (type == 1)
            {
                int id, cpu, mem;
                cin >> id >> cpu >> mem;
                Container cont(cpu, mem);
                containers[id] = cont;
                new_containers.push_back(id);
            }
            if (type == 2)
            {
                int id;
                cin >> id;
                shutdown_containers.push_back(id);
            }
        }

        for (auto vm = vms.begin(); vm != vms.end(); ++vm) {
            for (int ind : new_containers) {
                auto cont = containers.find(ind);
                if (cont->second.vm_ind == -1 && vm->second.free_cpu >= cont->second.cpu && vm->second.free_mem >= cont->second.mem) {
                    cont->second.vm_ind = vm->second.ind;
                    vm->second.free_cpu -= cont->second.cpu;
                    vm->second.free_mem -= cont->second.mem;

                    if (vm->second.start_time <= j) {
                        ++cnt_out;
                        output << 3 << ' ' << ind << ' ' << vm->second.ind << '\n';
                    } else
                        containers_to_allocate[vm->second.start_time].push_back(ind);
                }
            }
        }

        for (int ind : new_containers) {
            auto cont = containers.find(ind);
            if (cont->second.vm_ind != -1)
                continue;

            VM vm(find_vm_type(cont->second.cpu, cont->second.mem), j+d);
            vm.free_cpu -= cont->second.cpu;
            vm.free_mem -= cont->second.mem;
            vm.ind = cnt_vms;
            ++cnt_out;
            output << 1 << ' ' << cnt_vms << ' ' << vm.type+1  << '\n';
            vms[cnt_vms] = vm;
            cont->second.vm_ind = cnt_vms;
            ++cnt_vms;
            containers_to_allocate[j+d].push_back(ind);
        }
        new_containers.clear();

        for (int ind : shutdown_containers) {
            clear_resources(ind);
            containers.erase(ind);
        }
        shutdown_containers.clear();

        list<int> to_erase;
        for (auto vm : vms) {
            if (vm.second.free_cpu == vm_types[vm.second.type].cpu && vm.second.free_mem == vm_types[vm.second.type].mem) {
                to_erase.push_back(vm.second.ind);
            }
        }

        for(int ind : containers_to_allocate[j]) {
            output << 3 << ' ' << ind << ' ' << containers[ind].vm_ind << '\n';
            ++cnt_out;
        }

        cout << cnt_out << '\n';
        if (cnt_out != 0)
            cout << output.str();
        cnt_out = 0;
        output.str("");
        output.clear();

        for (auto ind : to_erase) {
            vms.erase(ind);
            output << 2 << ' ' << ind << '\n';
            ++cnt_out;
        }

        FOR (it, 1, cnt)
        {
            j++;

            for(int ind : containers_to_allocate[j]) {
                if (containers.find(ind) != containers.end()) {
                    output << 3 << ' ' << ind << ' ' << containers[ind].vm_ind << '\n';
                    ++cnt_out;
                }
            }

            cout << cnt_out << '\n';
            if (cnt_out != 0)
                cout << output.str();
            cnt_out = 0;
            output.str("");
            output.clear();

            containers_to_allocate.erase(j);
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



