#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <mutex>
#include <thread>

using std::condition_variable;
using std::mutex;
using std::thread;
using std::unique_lock;

const int max_thread_num = 39;

struct Task {
    int *a;
    int la;
    int ra;
    int rb;
};

Task *threads_tasks[max_thread_num];
int threads_working[max_thread_num];
mutex threads_mutexs[max_thread_num];
condition_variable threads_conds[max_thread_num];

static bool over = false;

void seq_merge(int *a, int la, int ra, int rb) {
    // printf("child merge, la, ra, rb: %d %d %d\n", la, ra, rb);
    int i = la, j = ra;
    int *c = new int[rb - la];
    int ci = 0;
    while (i < ra || j < rb) {
        if (i >= ra) {
            c[ci] = a[j];
            j += 1;
        } else if (j >= rb) {
            c[ci] = a[i];
            i += 1;
        } else {
            if (a[i] < a[j]) {
                c[ci] = a[i];
                i += 1;
            } else {
                c[ci] = a[j];
                j += 1;
            }
        }
        ci += 1;
    }
    for (i = 0; i < (rb - la); i++) {
        a[la + i] = c[i];
    }
    delete[] c;
}

void thread_job(int my_index) {
    while (true) {
        unique_lock<mutex> lock{threads_mutexs[my_index]};
        threads_conds[my_index].wait(
            lock, [my_index]() { return over || threads_tasks[my_index] != nullptr; });
        if (!over) {
            threads_working[my_index] = 1;
            Task *task = threads_tasks[my_index];
            if (task->la % 1000000 == 0) {
                printf("thread %d got: %d %d\n", my_index, task->la, task->rb);
            }
            seq_merge(task->a, task->la, task->ra, task->rb);
            threads_working[my_index] = 0;
            threads_tasks[my_index] = nullptr;
            delete task;
        } else {
            break;
        }
    }
}

void add_task(int index, int *a, int la, int ra, int rb) {
    unique_lock<mutex> lock{threads_mutexs[index]};
    threads_tasks[index] = new Task{a, la, ra, rb};
    threads_conds[index].notify_one();
}

void parallel_merge_sort(int *a, int n) {
    int step = 1;
    thread *threads[max_thread_num] = {};
    for (int i = 0; i < max_thread_num; i++) {
        threads[i] = new thread(thread_job, i);
    }
    while (step < n) {
        printf("step: %d\n", step);
        int sub_array_start = 0;
        int index = 0;
        while (sub_array_start < n) {
            // printf("into step, subarray_start: %d\n", sub_array_start);
            // Only one sub array left, nothing to do
            if (sub_array_start + step >= n) {
                sub_array_start = n;
            }
            // one and a part sub array
            else if (sub_array_start + (2 * step) >= n) {
                add_task(index, a, sub_array_start, sub_array_start + step, n);
                index += 1;
                sub_array_start = n;
            }
            // tow
            else {
                add_task(index, a, sub_array_start, sub_array_start + step,
                         sub_array_start + 2 * step);
                index += 1;
                sub_array_start += 2 * step;
            }
            if (index == max_thread_num || sub_array_start >= n) {
                while (true) {
                    bool done = true;
                    for (int i = 0; i < max_thread_num; i++) {
                        if (threads_tasks[i] != nullptr) {
                            done = false;
                            break;
                        }
                    }
                    if (done)
                        break;
                }
                index = 0;
            }
        }
        step *= 2;
    }
}

void merge_sort(int *a, int l, int r) {
    if (l + 1 >= r)
        return;
    int mid = (l + r) / 2;
    merge_sort(a, l, mid);
    merge_sort(a, mid, r);
    int i = l, j = mid;
    int *c = new int[r - l];
    int ci = 0;
    while (i < mid || j < r) {
        if (i >= mid) {
            c[ci] = a[j];
            j++;
        } else if (j >= r) {
            c[ci] = a[i];
            i++;
        } else {
            if (a[i] < a[j]) {
                c[ci] = a[i];
                i++;
            } else {
                c[ci] = a[j];
                j++;
            }
        }
        ci += 1;
    }
    for (i = 0; i < r - l; i++) {
        a[l + i] = c[i];
    }
    delete[] c;
}

/*

# generate data
import random

def run():
    n = 100000000
    with open("foo.txt", mode='w') as fp:
        for i in range(n):
            fp.write(str(random.randint(-1000000, 10000000)) + "\n")


run()

*/
int main(int argc, char const *argv[]) {
    // const int n = 40;
    // int a[n] = {719236, 525188, 408489, 420408, 220979, 698422, 416925, 775740, 479633, 892094,
    //             439659, 165225, 252751, 309200, 160968, 73911,  10269,  286254, 211835, 500573,
    //             903568, 708307, 135487, 113018, 625333, 875372, 547818, 902488, 267592, 379904,
    //             948712, 679451, 100246, 164718, 968861, 261727, 722424, 624482, 283542, 739829};
    // for (int i = 0; i < n; i++) {
    //     printf("%d ", a[i]);
    // }
    // printf("\n");

    if (argc == 2) {
        freopen(argv[1], "r", stdin);
    }

    const int max_n = 1000000000;
    int *a = new int[max_n];
    int *acopy = new int[max_n];
    int x = 0;
    int n = 0;
    while (scanf(" %d", &x) != EOF) {
        a[n] = acopy[n] = x;
        n += 1;
    }
    time_t par_start = time(nullptr);
    printf("parallel start\n");
    parallel_merge_sort(a, n);
    printf("parallel over, take %ld\n", time(nullptr) - par_start);
    for (int i = 1; i < n; i++) {
        if (a[i] < a[i - 1]) {
            printf("PARALLEL - NO OK\n");
            return EXIT_FAILURE;
        }
    }
    printf("PARALLEL - OK\n");
    time_t seq_start = time(nullptr);
    printf("sequential start\n");
    merge_sort(acopy, 0, n);
    printf("sequential over, take %ld\n", time(nullptr) - seq_start);
    for (int i = 1; i < n; i++) {
        if (acopy[i] < acopy[i - 1]) {
            printf("SEQUENTIAL - NO OK\n");
            return EXIT_FAILURE;
        }
    }
    printf("SEQUENTIAL - OK\n");
    // for (int i = 0; i < n; i++) {
    //     printf("%d ", a[i]);
    // }
    // printf("\n");
    return 0;
}
