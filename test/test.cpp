#include "queue.h"
#include <catch2/catch_test_macros.hpp>

#include <vector>
#include <iostream>
#include <chrono>
#include <atomic>

using namespace std::chrono;

TEST_CASE("Construct Queue (with copy constructor as well.)")
{
    Queue<int> queue(5);
    REQUIRE(queue.count() == 0);
    REQUIRE(queue.size() == 5);
    queue.push(2);
    queue.push(3);

    Queue<int> new_queue(queue);
    REQUIRE(queue.count() == 2);
    REQUIRE(queue.size() == 5);
    
    std::vector<int> data;
    for(int i = 0; i < new_queue.count(); i++)
        data.push_back(*(new_queue.data() + i));

    REQUIRE(data == std::vector<int>{2, 3});
}

TEST_CASE("Push to Queue: not full")
{
    Queue<int> queue(3);

    std::vector<int> expected_data{1, 2, 3};
    for (auto element : expected_data)
        queue.push(element);

    REQUIRE(queue.count() == queue.size());

    std::vector<int> obtained_data{};
    for (int i = 0; i < queue.count(); i++)
        obtained_data.push_back(*(queue.data() + i));

    REQUIRE(obtained_data == expected_data);
}

TEST_CASE("Push to Queue: full")
{
    Queue<int> queue(3);
    for (auto element : {1, 2, 3})
        queue.push(element);

    queue.push(10);

    std::vector<int> expected_data{2, 3, 10};
    std::vector<int> obtained_data{};

    for (int i = 0; i < queue.count(); i++)
        obtained_data.push_back(*(queue.data() + i));

    REQUIRE(obtained_data == expected_data);

    queue.push(25);
    queue.push(33);

    expected_data = {10, 25, 33};
    obtained_data.clear();

    for (int i = 0; i < queue.count(); i++)
        obtained_data.push_back(*(queue.data() + i));

    REQUIRE(obtained_data == expected_data);
}

TEST_CASE("Pop from Queue (using pop method): without timeout")
{
    Queue<int> queue(5);
    for (auto element : {1, 3, 2, 6})
        queue.push(element);

    REQUIRE(queue.count() == 4);

    int popped = queue.pop();
    std::vector<int> obtained_data{};
    for (int i = 0; i < queue.count(); i++)
        obtained_data.push_back(*(queue.data() + i));

    REQUIRE(popped == 1);
    REQUIRE(queue.count() == 3);
    REQUIRE(obtained_data == std::vector<int>{3, 2, 6});

    popped = queue.pop();
    obtained_data.clear();
    for (int i = 0; i < queue.count(); i++)
        obtained_data.push_back(*(queue.data() + i));

    REQUIRE(popped == 3);
    REQUIRE(queue.count() == 2);
    REQUIRE(obtained_data == std::vector<int>{2, 6});
}

template <typename T>
void read(Queue<T> &queue, std::vector<T> &elements)
{
    elements.push_back(queue.pop()); // pops 1
    std::this_thread::sleep_for(std::chrono::seconds(2));
    elements.push_back(queue.pop()); // pops 3
    elements.push_back(queue.pop()); // pops 4
    elements.push_back(queue.pop()); // blocks
    // -> 5 is added and this is released.
}

TEST_CASE("Write and Read from Queue, concurrently")
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
    reader.join();
    writer.join();

    REQUIRE(elements == std::vector<int>{1, 3, 4, 5});
    REQUIRE(queue.count() == 0); 
}

void thisThrows()
{
    Queue<int> queue(2);
    queue.popWithTimeout(100);
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

TEST_CASE("popWithTimeout throws exception")
{
    try
    {
        thisThrows();
        CHECK(false);
    }
    catch (const std::exception &e)
    {
        CHECK(true);
    }
}