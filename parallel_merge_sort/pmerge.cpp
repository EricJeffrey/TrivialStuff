#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <list>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

using namespace std::chrono_literals;
using std::condition_variable;
using std::list;
using std::mutex;
using std::queue;
using std::thread;
using std::unique_lock;

void seq_merge(int *a, int l, int mid, int r) {
    int *c = new int[r - l];
    int ci = 0;
    int i = l, j = mid;
    while (i < mid || j < r) {
        if (i >= mid) {
            c[ci] = a[j];
            j += 1;
        } else if (j >= r) {
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
    for (i = 0; i < r - l; i++) {
        a[l + i] = c[i];
    }
    delete[] c;
}

void seq_merge_sort(int *a, int l, int r) {
    if (l + 1 >= r)
        return;
    int mid = (l + r) / 2;
    seq_merge_sort(a, l, mid);
    seq_merge_sort(a, mid, r);
    seq_merge(a, l, mid, r);
}

struct Task {
    virtual void exec_task() = 0;
    virtual void pinfo() {}
};

struct TaskSeqMergeSort : public Task {
    int *a;
    int l;
    int r;
    TaskSeqMergeSort(int *a, int l, int r) : a(a), l(l), r(r) {}

    virtual void exec_task() override { seq_merge_sort(a, l, r); }
    void pinfo() override { printf("merge-sort task: %d %d\n", l, r); }
};

struct TaskSeqMerge : public Task {
    int *a;
    int l;
    int mid;
    int r;

    TaskSeqMerge(int *a, int l, int mid, int r) : a(a), l(l), mid(mid), r(r) {}

    virtual void exec_task() override { seq_merge(a, l, mid, r); }
    void pinfo() override { printf("merge task: %d %d %d\n", l, mid, r); }
};

class ThreadPool {
public:
    int max_thread_num;
    queue<Task *> task_queue;
    mutex queue_mutex;
    condition_variable queue_cond;

    thread *threads;
    bool over;

    int num_idle_thread;
    mutex num_idle_mutex;
    void increase_idle() {
        unique_lock<mutex> lock{num_idle_mutex};
        num_idle_thread += 1;
    }
    void decrease_idle() {
        unique_lock<mutex> lock{num_idle_mutex};
        num_idle_thread -= 1;
    }

    void thread_job() {
        while (!over) {
            Task *task = nullptr;
            {
                unique_lock<mutex> lock{queue_mutex};
                queue_cond.wait(lock, [this]() { return this->over || !this->task_queue.empty(); });
                if (over)
                    break;
                task = task_queue.front();
                task_queue.pop();
            }
            task->pinfo();
            decrease_idle();
            task->exec_task();
            delete task;
            increase_idle();
        }
    }

    ThreadPool(int max_thread_num) : max_thread_num(max_thread_num) {
        num_idle_thread = max_thread_num;
        over = false;
        threads = new thread[max_thread_num];
        for (int i = 0; i < max_thread_num; i++) {
            threads[i] = thread{&ThreadPool::thread_job, this};
        }
    }
    ~ThreadPool() {
        if (threads != nullptr) {
            over = true;
            for (int i = 0; i < max_thread_num; i++) {
                threads[i].join();
            }
            delete[] threads;
        }
    }

    void wait_for_all_idle() {
        printf("wait for idle\n");
        while (true) {
            unique_lock<mutex> lock{queue_mutex};
            if (task_queue.empty())
                break;
            std::this_thread::yield();
        }
        printf("wait for idle, task empty\n");
        // now queue empty, wait for all task done
        while (true) {
            unique_lock<mutex> lock{num_idle_mutex};
            if (num_idle_thread == max_thread_num)
                break;
            std::this_thread::yield();
        }
        printf("wait for idle, all idle\n");
    }

    // wait for queue empty then stop
    void stop() {
        wait_for_all_idle();
        over = true;
        queue_cond.notify_all();
        for (int i = 0; i < max_thread_num; i++) {
            threads[i].join();
        }
        delete[] threads;
        threads = nullptr;
    }

    void add_task(Task *task) {
        unique_lock<mutex> lock{queue_mutex};
        task_queue.push(task);
        queue_cond.notify_one();
    }
};

void pmerge(int *a, int l, int r) {
    static const int max_thread_num = 40;
    static const int max_seq_block_size = 100000;
    ThreadPool pool{max_thread_num};
    int n = r - l;
    // sequential sort for every block
    for (int i = 0; i < r; i += max_seq_block_size) {
        int tmpr = i + max_seq_block_size;
        if (tmpr > r)
            tmpr = r;
        pool.add_task(new TaskSeqMergeSort(a, i, tmpr));
    }
    // then merge
    int step = max_seq_block_size;
    pool.wait_for_all_idle();
    printf("do merge\n");
    while (step < n) {
        printf("step: %d\n", step);
        int start = 0;
        while (start < r) {
            if (start + step >= r) {
                // nothing to do for the last single sorted block
                break;
            } else if (start + step * 2 >= r) {
                pool.add_task(new TaskSeqMerge(a, start, start + step, r));
            } else {
                pool.add_task(new TaskSeqMerge(a, start, start + step, start + step * 2));
            }
            start += step * 2;
        }
        pool.wait_for_all_idle();
        step *= 2;
    }
    pool.stop();
}

int main(int argc, char const *argv[]) {
    if (argc >= 2) {
        freopen(argv[1], "r", stdin);
    }
    FILE *out = stdout;
    if (argc >= 3) {
        out = fopen(argv[2], "w");
    }

    const int64_t max_n = 1000000000;
    int *a = new int[max_n];
    int x = 0;
    int n = 0;
    while (scanf(" %d", &x) != EOF) {
        a[n] = x;
        n += 1;
    }
    time_t par_start = time(nullptr);
    printf("parallel start\n");
    pmerge(a, 0, n);
    printf("parallel over, take %ld\n", time(nullptr) - par_start);
    bool ok = true;
    int failure_index = 0;
    for (int i = 1; i < n; i++) {
        if (a[i] < a[i - 1]) {
            ok = false;
            failure_index = i;
            break;
        }
    }
    if (ok)
        printf("PARALLEL - OK\n");
    else
        printf("PARALLEL - NO OK, fail at %d, with: %d %d %d\n", failure_index,
               a[failure_index - 1], a[failure_index], a[failure_index + 1]);
    fflush(stdout);
    for (int i = 0; i < n; i++) {
        fprintf(out, "%d\n", a[i]);
    }
    fclose(out);
    return 0;
}
