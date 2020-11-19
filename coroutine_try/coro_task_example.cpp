
#include <unistd.h>

#include <coroutine>
#include <functional>
#include <future>
#include <iostream>
#include <string>
#include <thread>

using std::cout, std::endl;
using std::string;

using TaskRetType = double;

template <typename RetType> struct CoroRetValue {
    using job_type = std::packaged_task<RetType()>;

    struct result_wrapper;
    struct awaitable;
    struct promise_type;

    job_type job;
    std::shared_ptr<result_wrapper> resultPtr;

    CoroRetValue(promise_type *promise) : resultPtr(std::make_shared<result_wrapper>()) {
        promise->set_return_object(this);
    }
    ~CoroRetValue() {}

    void setJob(job_type &&j) { job = std::move(j); }

    struct result_wrapper {
        RetType value;
        std::exception_ptr error;
        bool hasValue;
        bool hasError;

        result_wrapper() : error(nullptr), hasValue(false), hasError(false) {}
        void setValue(const RetType &v) {
            value = v;
            hasValue = true;
        }
        void setError(std::exception_ptr err) {
            error = err;
            hasError = true;
        }
    };

    struct promise_type {
        CoroRetValue *returnObject = nullptr;

        promise_type() {}
        ~promise_type() {}

        void set_return_object(CoroRetValue *retObj) { returnObject = retObj; }

        std::suspend_never initial_suspend() noexcept { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }

        CoroRetValue get_return_object() { return CoroRetValue{this}; }
        void return_void() {}

        void unhandled_exception() { returnObject->resultPtr->setError(std::current_exception()); }

        awaitable await_transform(job_type &&job) {
            returnObject->setJob(std::move(job));
            return awaitable{returnObject};
        }
    };

    struct awaitable {
        CoroRetValue *coro;

        awaitable(CoroRetValue *coro) : coro(coro) {}
        bool await_ready() { return false; }
        void await_suspend(std::coroutine_handle<> handle) {
            std::thread([handle, this]() {
                try {
                    auto resFuture = coro->job.get_future();
                    coro->job();
                    coro->resultPtr->setValue(resFuture.get());
                } catch (...) {
                    coro->resultPtr->setError(std::current_exception());
                }
                handle.resume();
            }).detach();
        }
        result_wrapper await_resume() { return *(coro->resultPtr); }
    };
};

template <typename Callable, typename... Args>
auto make_packaged(Callable &&fn, Args &&... args) -> std::packaged_task<decltype(fn(args...))()> {
    return std::packaged_task<decltype(fn(args...))()>(std::bind(fn, args...));
}

CoroRetValue<TaskRetType> coroA(int x) {
    cout << "coroA: " << x << endl;
    auto result = co_await make_packaged([x](double y) -> TaskRetType { return 3 * y + x; }, 3.0);
    cout << result.value << endl;
    cout << result.error.operator bool() << endl;

    // auto s = make_packaged(
    //     [](double x) -> string {
    //         cout << "ok" << endl;
    //         return "sdf";
    //     },
    //     3.0);
    // s();
    // return {};
}

/*


协程的调用方式

Task<Type> coroA(){
    ......
    Type result = co_await bind(async_job, arg1, arg2);
    // 协程暂停，返回调用方
    ......
}

主线程某个函数
void funcInMainThread() {
    ......
    coroA();
    ......
}

 */

int main(int argc, char const *argv[]) {
    coroA(1);
    usleep(400000);
    coroA(2);
    while (true) {
    }
    return 0;
}
