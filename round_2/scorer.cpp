#include "bits/stdc++.h"

#include "algotester.h"

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



const double PI = acos(-1.0);

const int INF = 1000 * 1000 * 1000 + 7;

const LL LINF = INF * (LL) INF;



const int MAX_CONTAINER_ALLOCATION_TIME = 4774;



VI vm_prices;



struct VM

{

    int start_time;

    int end_time;

    int type;



    VM()

    {

        start_time = -1;

        end_time = -1;

        type = -1;

    }

};



struct Container

{

    int start_time;

    int allocation_time;



    int shutdown_time = -1;

    int cpu;

};



double best_price = 1e47;



void read_test(AlgotesterReader& in)

{

    int m = in.readInt();

    in.readInt();

    vm_prices.resize(m);



    FOR (i, 0, m)

    {

        int cpu = in.readInt();

        in.readInt();

        int price = in.readInt();



        vm_prices[i] = price;



        best_price = min(best_price, price / 1e4 / cpu);

    }

}



int main(int argc, char* argv[])

{

	//ios::sync_with_stdio(false); cin.tie(0);



    auto [test_in, checker_out, ans] = initScorer(argc, argv);



    read_test(test_in);



    map<int, Container> containers;

    map<int, VM> vms;



    VI delays;



    int t = checker_out.readInt();

    FOR (j, 0, t)

    {

        int e = checker_out.readInt();

        int cnt = 1;

        if (e == 0) cnt = checker_out.readInt();



        FOR (k, 0, e)

        {

            int type = checker_out.readInt();

            if (type == 1)

            {

                int cont_id = checker_out.readInt();

                int cpu = checker_out.readInt();

                checker_out.readInt();



                containers[cont_id].start_time = j;

                containers[cont_id].cpu = cpu;



            }

            if (type == 2)

            {

                int cont_id = checker_out.readInt();

                containers[cont_id].shutdown_time = j;

            }

        }



        j--;

        FOR (it, 0, cnt)

        {

            j++;

            int a = checker_out.readInt();

            FOR (k, 0, a)

            {

                int type = checker_out.readInt();

                if (type == 1)

                {

                    int vm_id = checker_out.readInt();

                    int vm_type = checker_out.readInt();



                    vms[vm_id].start_time = j;

                    vms[vm_id].type = vm_type - 1;

                }

                if (type == 2)

                {

                    int vm_id = checker_out.readInt();



                    vms[vm_id].end_time = j;

                }

                if (type == 3)

                {

                    int cont_id = checker_out.readInt();

                    checker_out.readInt();



                    containers[cont_id].allocation_time = j;

                }

            }

        }

    }



    double reservation_cost = 0;



    ITER(it, vms)

    {

        if (it->second.end_time == -1) it->second.end_time = t + 1;

        reservation_cost += (it->second.end_time - it->second.start_time) * (double)vm_prices[it->second.type] / 1e4;

    }



    double delay_cost = 0;

    double total_cpu = 0;



    ITER(it, containers)

    {

        delay_cost += pow(1.1, min(it->second.allocation_time - it->second.start_time, MAX_CONTAINER_ALLOCATION_TIME)) - 1;

        if (it->second.shutdown_time == -1) it->second.shutdown_time = t + 1;



        total_cpu += (it->second.shutdown_time - it->second.allocation_time) * (double)it->second.cpu;

    }



    LL score = (LL)((total_cpu * best_price) / (reservation_cost + delay_cost * 10) * 1e7);



    cout<<score<<endl;





}
