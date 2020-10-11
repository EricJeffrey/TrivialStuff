#ifndef __cpp_impl_coroutine
#define __cpp_impl_coroutine 1
#endif

#include <coroutine>
#include <ctime>
#include <functional>
#include <iostream>
#include <thread>
#include <unistd.h>

using std::cin, std::cout, std::cerr, std::endl;

/*

协程执行顺序

用 operator new 分配协程状态对象（见下文）
将所有函数形参复制到协程状态中？
调用承诺对象的构造函数，按形参匹配或默认构造函数
调用 promise.get_return_object() 该调用的结果将在协程首次暂停时返回给调用方
调用 promise.initial_suspend() 并 co_await 其结果。 std::suspend_always/std::suspend_never。
当 co_await promise.initial_suspend() 恢复时，开始协程体的执行
...
co_yield expr = co_await promise.yield_value(expr)
co_await expr ->
    将expr转换为可等待体 awaitable - expr本身 或 promise.await_transform(expr) 的值
    调用 awaitable 的 [operator co_await] (成员或非成员) 得到等待体 awaiter
        若无成员则为 awaitable 是 awaiter
    调用 awaiter.await_ready()，若返回 false
        暂停协程
        调用 awaiter.await_suspend(handle), handle 是表示当前协程的协程句柄，获得返回值
            若返回void - 控制立即返回调用方/恢复方
            若返回bool值
                true - 控制返回调用方/恢复方
                false - 恢复当前协程
            若返回其他协程handle - 调用 handle.resume() 恢复其
            若抛出异常 - 捕捉异常，恢复携程并立即重新抛出
        调用 awaiter.await_resume() - 结果为co_await expr的结果

...
co_return ->
    调用 promise.return_void() / promise.return_value()
    销毁自动存储的变量
    调用 promise.final_suspend() 并 co_wait

*/

void logger(const char *msg) { cerr << msg << endl; }

struct Generator {
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    handle_type handle;

    struct promise_type {
        int current_value;

        promise_type() : current_value(0) {}
        inline auto get_return_object() {
            logger("auto get_return_object() ");
            return Generator{handle_type::from_promise(*this)};
        }
        auto initial_suspend() {
            logger("auto initial_suspend() ");
            return std::suspend_never();
        }
        // what if suspend_never ?
        auto final_suspend() noexcept {
            logger("auto final_suspend() noexcept ");
            return std::suspend_always();
        }
        void return_void() { logger("void return_void() "); }
        auto yield_value(int value) {
            logger("auto yield_value(int value) ");
            current_value = value;
            return std::suspend_always();
        }
        auto unhandled_exception() {
            logger("auto unhandled_exception() ");
            std::terminate();
        }
    };

    // constructor
    Generator(handle_type handle) : handle(handle) {}
    Generator(const Generator &) = delete;
    ~Generator() {
        logger("~Generator()");
        if (handle)
            handle.destroy();
    }

    bool next() {
        logger("bool next() ");
        return (handle ? (handle.resume(), !handle.done()) : false);
    }
    int value() {
        logger("int value() ");
        return handle.promise().current_value;
    }
};

/*
g++ -g -Wall -o main.out main.cpp -std=c++20 -fcoroutines
*/
Generator g() {
    logger("Generator g() ");
    for (int i = 7; i < 20; i += 3) {
        co_yield i;
    }
}

int main() {
    logger("int main() ");
    auto generator = g();

    while (generator.next()) {
        cout << generator.value() << endl;
    }
}
