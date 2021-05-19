#include <cstdio>
#include <cstdlib>
#include <thread>

using std::thread;

const int max_thread_num = 20;

void parallel_merge(int *a, int la, int ra, int rb) {
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

void parallel_merge_sort(int *a, int n) {
    int step = 1;
    while (step < n) {
        // printf("step: %d\n", step);
        int sub_array_start = 0;
        thread *threads[max_thread_num] = {};
        int index = 0;
        while (sub_array_start < n) {
            // printf("into step, subarray_start: %d\n", sub_array_start);
            // Only one sub array left, nothing to do
            if (sub_array_start + step >= n) {
                sub_array_start = n;
            }
            // one and a part sub array
            else if (sub_array_start + (2 * step) >= n) {
                threads[index] =
                    new thread(parallel_merge, a, sub_array_start, sub_array_start + step, n);
                index += 1;
                sub_array_start = n;
            }
            // tow
            else {
                threads[index] = new thread(parallel_merge, a, sub_array_start,
                                            sub_array_start + step, sub_array_start + 2 * step);
                index += 1;
                sub_array_start += 2 * step;
            }
            if (index == 7 || sub_array_start >= n) {
                for (int i = 0; i < index; i++) {
                    threads[i]->join();
                    delete threads[i];
                }
                index = 0;
            }
        }
        step *= 2;
    }
}

/*

# generate data
import random

def run():
    n = 100000
    with open("foo.txt", mode='w') as fp:
        for i in range(n):
            fp.write(str(random.randint(-1000000, 10000000)) + " ")


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

    const int max_n = 100000000;
    int *a = new int[max_n];
    int x = 0;
    int n = 0;
    while (scanf(" %d", &x) != EOF) {
        a[n] = x;
        n += 1;
    }
    parallel_merge_sort(a, n);
    for (int i = 1; i < n; i++) {
        if (a[i] < a[i - 1]) {
            printf("NO OK\n");
            return EXIT_FAILURE;
        }
    }
    printf("OK\n");
    // for (int i = 0; i < n; i++) {
    //     printf("%d ", a[i]);
    // }
    // printf("\n");
    return 0;
}
