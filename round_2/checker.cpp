#include <iostream>
#include <utility>
#include <vector>
#include <map>
#include <set>
#include <tuple>
#include "algotester.h"
#include "algotester_generator.h"
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

const int TIME_AT_THE_END = 4774;

const int MAX_ACTIONS = 30'000'000;
const int MAX_VM_ID = 1'000'000'000;
const int MAX_CONT_ID = 1'000'000'000;

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

struct VM
{
    int cpu_used;
    int mem_used;
    int start_time;
    int shutdown_time;
    int vm_type;

    VM(){}
    VM(int start_time, int vm_type)
    {
        this->start_time = start_time;
        this->shutdown_time = -1;

        this->vm_type = vm_type;
        this->cpu_used = 0;
        this->mem_used = 0;
    }
};

struct Container
{
    int cpu;
    int mem;
    int time;
    int duration;
    int id;

    Container(){}
    Container(int cpu, int mem, int time, int duration, int id)
    {
        this->cpu = cpu;
        this->mem = mem;
        this->time = time;
        this->duration = duration;
        this->id = id;
    }
};

struct Event
{
    int type;
    int time;
    int container_id;

    Event(int type, int time, int container_id)
    {
        this->type = type;
        this->time = time;
        this->container_id = container_id;
    }
};

int m, d;
vector<VMType> vm_types;
map<int, vector<Event> > events;
map<int, Container> containers;
int max_time = 0;

void read_test(AlgotesterReader& in)
{
    m = in.readInt();
    d = in.readInt();
    vm_types.clear();

    FOR (i, 0, m)
    {
        int cpu = in.readInt();
        int mem = in.readInt();
        int price = in.readInt();

        vm_types.PB(VMType(cpu, mem, price));
    }

    int n = in.readInt();
    FOR (i, 0, n)
    {
        int time = in.readInt();
        int id = in.readInt();
        int cpu = in.readInt();
        int mem = in.readInt();
        int duration = in.readInt();

        events[time].PB(Event(1, time, id));
        containers[id] = Container(cpu, mem, time, duration, id);

        max_time = max(max_time, time + duration);
    }
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false); //cin.tie(0);

    auto gen = initGenerator(argc, argv);

    auto [test_in, user_in, user_out] = initInteractiveChecker(argc, argv);
    auto output = ofstream(argv[5]);

    auto error = [&user_in](){user_in << -1 << endl;};
    user_out.setErrorCallback(error);

    auto checkWithError = [&error](bool condition, string message) {check(condition, message, error);};

    read_test(test_in);

    user_in << m << ' ' << d << "\n";
    FOR (i, 0, m)
    {
        user_in << vm_types[i].cpu << ' ' << vm_types[i].mem << ' ' << vm_types[i].price << "\n";
    }

    int t = max_time + TIME_AT_THE_END;
    user_in << t << "\n";
    output << t << "\n";
    // cerr << t << "\n";

    set<int> unallocated_containers;
    int actions_count = 0;
    map<int, VM> vms;
    map<int, int> container_vm;

    FOR (j, 0, t)
    {
        vector<int> containers_to_shutdown;
        int cnt = 1;
        if (events.count(j) == 0)
        {
            auto iter = events.lower_bound(j);
            int r = iter == events.end() ? t : iter -> first;
            cnt = gen.randInt(1, min(20, r - j));
            user_in << "0 " << cnt << "\n";
            output << "0 " << cnt << "\n";
            // cerr << "0 " << cnt << "\n";
        }
        else
        {
            vector<Event>& e = events[j];
            user_in << SZ(e) << "\n";
            output << SZ(e) << "\n";
            // cerr << SZ(e) << "\n";
            FOR (k, 0, SZ(e))
            {
                if (e[k].type == 1)
                {
                    user_in << "1 " << e[k].container_id << ' ' << containers[e[k].container_id].cpu << ' ' << containers[e[k].container_id].mem << "\n";
                    output << "1 " << e[k].container_id << ' ' << containers[e[k].container_id].cpu << ' ' << containers[e[k].container_id].mem << "\n";
                    // cerr << "1 " << e[k].container_id << ' ' << containers[e[k].container_id].cpu << ' ' << containers[e[k].container_id].mem << "\n";
                    unallocated_containers.insert(e[k].container_id);
                }
                else
                {
                    user_in << "2 " << e[k].container_id << "\n";
                    output << "2 " << e[k].container_id << "\n";
                    // cerr << "2 " << e[k].container_id << "\n";
                    containers_to_shutdown.PB(e[k].container_id);
                }
            }
        }

        user_in.flush();

        j--;
        FOR (it, 0, cnt)
        {
            j++;
            set<int> vms_to_shutdown;

            int a = user_out.readInt(0, MAX_ACTIONS - actions_count, "a_j");
            output << a << "\n";
            // cerr << "==> " << a << "\n";
            actions_count += a;

            FOR (k, 0, a)
            {
                int type = user_out.readInt(1, 3, "type_j");
                if (type == 1)
                {
                    int id = user_out.readInt(1, MAX_VM_ID, "id^{vm}_{jk}");
                    int vm_type = user_out.readInt(1, m, "type^{vm}_{jk}");

                    output << "1 " << id << ' ' << vm_type << "\n";
                    // cerr << "==> 1 " << id << ' ' << vm_type << "\n";

                    checkWithError(vms.count(id) == 0, "VM with such identifier has already been created");
                    vms[id] = VM(j, vm_type - 1);
                }
                if (type == 2)
                {
                    int id = user_out.readInt(1, MAX_VM_ID, "id^{vm}_{jk}");

                    output << "2 " << id << "\n";
                    // cerr << "==> 2 " << id << "\n";
                    
                    checkWithError(vms.count(id) != 0, "VM with such identifier has not been created yet");
                    
                    auto& vm = vms[id];
                    checkWithError(vm.shutdown_time == -1, "VM can not be shut down because it is has been already shut down");
                    checkWithError(vms_to_shutdown.count(id) == 0, "VM has already been scheduled to shut down");
                    checkWithError(vm.cpu_used == 0 && vm.mem_used == 0, "VM can not be shut down because it hosts containers");
                    
                    vms_to_shutdown.insert(id);
                }
                if (type == 3)
                {
                    int id_cont = user_out.readInt(1, MAX_CONT_ID, "id^{cont}_{jk}");
                    int id_vm = user_out.readInt(1, MAX_VM_ID, "id^{vm}_{jk}");

                    output << "3 " << id_cont << ' ' << id_vm << "\n";
                    // cerr << "==> 3 " << id_cont << ' ' << id_vm << "\n";
                    checkWithError(unallocated_containers.count(id_cont) != 0, "No such container request, or it has already been allocated");
                    unallocated_containers.erase(id_cont);
                    auto container = containers[id_cont];

                    checkWithError(vms.count(id_vm) != 0, "VM with such identifier has not been created yet");
                    auto& vm = vms[id_vm];
                    checkWithError(vm.shutdown_time == -1, "VM has already been shut down");
                    checkWithError(vms_to_shutdown.count(id_vm) == 0, "VM is scheduled to shut down on this step");
                    checkWithError(j >= vm.start_time + d, "VM is not ready to host containers");
                    checkWithError(vm.cpu_used + container.cpu <= vm_types[vm.vm_type].cpu, "VM doesn't have enough CPUs to host container");
                    checkWithError(vm.mem_used + container.mem <= vm_types[vm.vm_type].mem, "VM doesn't have enough memory to host container");

                    vm.cpu_used += container.cpu;
                    vm.mem_used += container.mem;
                    container_vm[id_cont] = id_vm;

                    events[j + container.duration].PB(Event(2, j + container.duration, id_cont));
                }
            }

            FOR (k, 0, SZ(containers_to_shutdown))
            {
                int id_cont = containers_to_shutdown[k];
                int id_vm = container_vm[id_cont];

                auto& vm = vms[id_vm];
                vm.cpu_used -= containers[id_cont].cpu;
                vm.mem_used -= containers[id_cont].mem;
            }

            ITER(it, vms_to_shutdown)
            {
                vms[*it].shutdown_time = j;
            }
        }
    }

    checkWithError(SZ(unallocated_containers) == 0, "Not all containers have been allocated");

    user_in << 0 << "\n";
    output << 0 << "\n";
    // cerr << 0 << "\n";
    
    user_in.flush();
    
    user_out.readEof();
}


