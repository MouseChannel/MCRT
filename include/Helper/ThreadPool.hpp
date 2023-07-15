#include <condition_variable>
#include <future>
#include <iostream>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
private:
    std::condition_variable avaliable;
    std::mutex myMutex;
    std::queue<std::function<void()>> taskQueue;
    bool stopped;

    int workerNum;

    std::vector<std::unique_ptr<std::thread>> workers;
    void StartSingleWorker();
    void StopWorks();
    void StopSingleWorker();

public:
    ThreadPool();

    auto Run(auto&& action, auto&&... args)
        -> std::future<decltype(action(args...))>;

    ~ThreadPool();
};

inline ThreadPool::ThreadPool()
{
    this->workerNum = std::thread::hardware_concurrency();
    for (size_t i = 0; i < (size_t)workerNum; i++) {

        StartSingleWorker();
    }
}

inline void ThreadPool::StartSingleWorker()
{
    auto threadPtr = std::make_unique<std::thread>([this]() {
        while (true) {

            auto stopped = std::bind(
                [this]() { return this->stopped || !(this->taskQueue).empty(); });

            std::function<void()> task;

            {
                std::unique_lock<std::mutex> lock(this->myMutex);

                this->avaliable.wait(lock, stopped);

                if (this->stopped && this->taskQueue.empty()) {
                    break;
                }

                task = std::move(this->taskQueue.front());

                this->taskQueue.pop();
            }

            task();
        }
    }

    );

    this->workers.emplace_back(std::move(threadPtr));
}

auto ThreadPool::Run(auto&& action, auto&&... args)
    -> std::future<decltype(action(args...))>
{
    using returnType = decltype(action(args...));

    auto task = make_shared<std::packaged_task<returnType()>>(
        std::bind(std::forward<decltype(action)>(action),
                  std::forward<decltype(args)>(args)...));

    this->taskQueue.emplace([task]() { (*task)(); });

    this->avaliable.notify_one();

    return task->get_future();
}

inline ThreadPool::~ThreadPool()
{

    std::cout << "thread delete" << std::endl;
    this->avaliable.notify_all();
    this->StopWorks();
}

inline void ThreadPool::StopWorks()
{
    this->stopped = true;
    for (auto& i : this->workers) {
        i->join();
    }
}
