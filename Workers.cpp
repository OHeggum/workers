#include <iostream>
#include <thread>
#include <vector>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <chrono>

class Workers {
public:
    explicit Workers(int thread_count) : thread_count(thread_count), stopFlag(false) {}

    std::vector<std::thread> threads;
    std::vector<std::function<void()>> functions;
    std::mutex functionsMutex;
    int thread_count;
    std::condition_variable condition;
    bool stopFlag;

    void start() {
        auto workerFunc = [this]() {
            while (true) {
                std::function<void()> func;
                {
                    std::unique_lock<std::mutex> lock(functionsMutex);
                    condition.wait(lock, [this] { return stopFlag || !functions.empty(); });
                    if (stopFlag && functions.empty()) {
                        return;
                    }
                    if (!functions.empty()) {
                        func = std::move(functions.back());
                        functions.pop_back();
                    }
                }
                if (func) {
                    func();
                }
            }
        };
        for (int i = 0; i < thread_count; ++i) {
            threads.emplace_back(workerFunc);
        }
    }

    void post(std::function<void()>* function) {
        std::lock_guard<std::mutex> lock(functionsMutex);
        functions.push_back(*function);
        condition.notify_one();
    }

    void stop() {
        stopFlag = true;
        condition.notify_all();

        for (auto &thread : threads) {
            thread.join();
        }
    }

    void post_timeout(int ms, std::function<void()>* function) {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
        post(function);
        condition.notify_one();
    }
};
