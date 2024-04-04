
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <inttypes.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <sys/time.h>

void run_test_float_scalar();

void run_test_float_packed();

void setcpuaff() {
    HANDLE hProcess = GetCurrentProcess();
    DWORD_PTR processAffinityMask = 0x1;
    WINBOOL res = SetProcessAffinityMask(hProcess, processAffinityMask);
    if (!res) {
        printf("set affinity failed: %d\n", GetLastError());
    }
}

int main(){
    setcpuaff();
    run_test_float_scalar();
    // run_test_float_packed();
    return 0;
}
