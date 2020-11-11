#include <chrono>
#include <coroutine>
#include <functional>
#include <future>
#include <iostream>
#include <string>
#include <thread>

using namespace std::chrono_literals;

using std::async, std::thread, std::future;
using std::cout, std::endl;
using std::function;
using std::string, std::optional;
using std::suspend_always, std::suspend_never;

// task with int32_t as return value
struct Task {
    struct Promise;
    using promise_type = Promise;

    struct Promise {
        suspend_never initial_suspend() { return {}; }
        suspend_never final_suspend() noexcept { return {}; }
        Task get_return_object() { return {}; }
        void return_void() {}
        void unhandled_exception() {}
    };

    Task() {}
    ~Task() {}
};

// awaitable async job that return int
struct CoroAsyncJob {
    // need to make it a template
    using JobType = function<int32_t()>;
    JobType job;
    optional<int32_t> res;

    CoroAsyncJob(JobType job) : job(job) {}

    CoroAsyncJob operator()() { return *this; }

    bool await_ready() { return res.has_value(); }
    void await_suspend(std::coroutine_handle<> h) {
        // should return handle to caller
        thread([h, this]() {
            // what if throw?
            res = job();
            h.resume();
        }).detach();
    }
    int32_t await_resume() { return res.value(); }
};

// this is a coroutine
Task coroA(int32_t index) {
    auto res = co_await CoroAsyncJob([index]() {
        int32_t res = 500 + index * 400;
        std::this_thread::sleep_for(std::chrono::milliseconds(res));
        return res * 1000;
    })();
    cout << "task " << index << " done, result: " << res << endl;
}

int main(int32_t argc, char const *argv[]) {
    for (int32_t i = 0; i < 10; i++) {
        coroA(i);
    }
    while (true) {
        cout << "main running" << endl;
        std::this_thread::sleep_for(200ms);
    }
    return 0;
}
