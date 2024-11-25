#include "queue.h"

#include <iostream>
#include <chrono>
#include <vector>
#include <thread>

template <typename T>
void read(Queue<T> &queue, std::vector<T> &elements)
{
    elements.push_back(queue.pop()); // pops 1
    std::this_thread::sleep_for(std::chrono::seconds(2));
    elements.push_back(queue.pop()); // pops 3
    elements.push_back(queue.pop()); // pops 4
    elements.push_back(queue.pop()); // blocks
}

int main()
{
    Queue<int> queue(2);
    std::vector<int> elements;

    std::thread reader(read<int>, std::ref(queue), std::ref(elements));

    std::thread writer([&queue]()
                       {
                        queue.push(1);
                        std::this_thread::sleep_for(std::chrono::seconds(1));
                        queue.push(2);
                        queue.push(3);
                        queue.push(4);
                        std::this_thread::sleep_for(std::chrono::seconds(5));
                        queue.push(5);
                        });

    std::this_thread::sleep_for(std::chrono::seconds(2));

    reader.join();
    writer.join();

    std::cout << "Number of elements in queue:" << queue.count();
    return 0;
}