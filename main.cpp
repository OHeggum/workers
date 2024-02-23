#include <iostream>
#include "Workers.cpp"

int main() {
    Workers workers(3);


    workers.start();
    std::function<void()> task1 = []() { std::cout << "Task 1 executed" << std::endl; };
    std::function<void()> task2 = []() { std::cout << "Task 2 executed" << std::endl; };
    std::function<void()> task3 = []() { std::cout << "Task 3 executed" << std::endl; };
    std::function<void()> task4 = []() { std::cout << "Task 4 executed" << std::endl; };

    workers.post(&task1);
    workers.post(&task2);
    workers.post(&task3);

    workers.post_timeout(10000,&task4);

    std:: cout << "all tasks done" <<std::endl;

    workers.stop();

    return 0;
}

