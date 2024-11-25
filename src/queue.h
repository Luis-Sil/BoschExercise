#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <mutex>
#include <condition_variable>
#include <chrono>

/**
 * @brief A thread-safe queue class.
 * 
 * This class implements a multi-threaded queue designed for concurrent
 * writing and reading operations.
 * 
 * @tparam T The type of elements stored in the queue.
 */
template <typename T>
class Queue
{
public:
    Queue() = delete; ///< Deleted default constructor to enforce size specification.

    /**
     * @brief Constructs a queue with a specified capacity.
     * 
     * Allocates memory for the queue with the given capacity.
     * 
     * @param size The maximum number of elements that the queue can hold.
     */
    Queue(int size) : m_filled(), m_data(nullptr), m_capacity(size)
    {
        // allocate without constructing
        m_data = static_cast<T *>(operator new(size * sizeof(T)));
    }

    /**
     * @brief Copy constructor.
     * 
     * Creates a copy of the specified Queue object.
     * 
     * @param src The Queue object to copy from.
     */
    Queue(const Queue &src) : m_filled(src.m_filled), m_data(nullptr), m_capacity(src.m_capacity)
    {
        m_data = static_cast<T *>(operator new(m_capacity * sizeof(T)));
        for (int i = 0; i < m_filled; i++)
            new (m_data + i) T(*(src.m_data + i));
    }

    /**
     * @brief Destructor.
     * 
     * Destroys the queue and its elements, releasing allocated memory.
     */
    ~Queue()
    {
        // destroy objects that exist
        for (int i = 0; i < m_filled; i++)
            (m_data + i)->~T();

        // free all space
        operator delete(m_data);

        cv.notify_all();
    }

    /**
     * @brief Adds a new element to the queue.
     * 
     * Inserts a new element into the queue. If the queue is full, the oldest
     * element is removed to make room for the new element.
     * 
     * @param element The element to add to the queue.
     */
    void push(const T& element)
    {
        std::unique_lock<std::mutex> lck(mtx);
        if (m_filled < m_capacity)
        {
            new (m_data + m_filled) T(element);
            m_filled += 1;
        }
        else
        {
            // the queue is full, so we copy all elements to the previous
            // position
            for (int i = 1; i < m_filled; i++)
                *(m_data + i - 1) = *(m_data + i);

            *(m_data + m_filled - 1) = element;
        }

        // a new element is added, so notify the reader thread
        cv.notify_one();
    }

    /**
     * @brief Removes and returns the oldest element in the queue.
     * 
     * Waits indefinitely until an element is available in the queue, then
     * removes and returns the oldest element.
     * 
     * @return The oldest element in the queue.
     */
    T pop()
    {
        // get stuck while there's no new elements
        std::unique_lock<std::mutex> lck(mtx);
        cv.wait(lck, [this]()
                { return m_filled != 0; });
        
        // take 1st element and copy all the others to the previous position
        T popped = *(m_data);
        for (int i = 1; i < m_filled; i++)
            *(m_data + i - 1) = *(m_data + i);

        // clear the last one and decrease the number of elements in the queue
        (m_data + m_filled)->~T();
        m_filled -= 1;

        return popped;
    }

    /**
     * @brief Removes and returns the oldest element in the queue, with a timeout.
     * 
     * Waits for a specified period for an element to be available in the queue.
     * If the timeout period elapses without an element being available, an
     * exception is thrown.
     * 
     * @param milliseconds_val The timeout period in milliseconds.
     * 
     * @return The oldest element in the queue.
     * 
     * @throws std::system_error If the timeout period elapses.
     */
    T popWithTimeout(int milliseconds_val)
    {
        // get stuck while there's no elements
        std::unique_lock<std::mutex> lck(mtx);
        bool not_empty = cv.wait_for(lck, std::chrono::milliseconds(milliseconds_val), [this]()
                                     { return m_filled != 0; });

        // queue is still empty and lock was freed.
        if (!not_empty)
            throw std::system_error{std::make_error_code(std::errc::operation_would_block),
                                    "Queue: pop() timeout"};

        // pop the oldest element.
        T popped = *(m_data);
        for (int i = 1; i < m_filled; i++)
            *(m_data + i - 1) = *(m_data + i);

        (m_data + m_filled)->~T();
        m_filled -= 1;

        return popped;
    }

    /**
     * @brief Number of Queue elements getter.
     * 
     * Returns the current number of elements in the Queue.
     * 
     * @return int Number of element in the Queue.
     */
    int count() const { return m_filled; } // Amount of elements stored now

    /**
     * @brief Queue capacity getter.
     * 
     * Returns the capacity of the queue.
     * 
     * @return int Capacity of the queue.
     */
    int size() const { return m_capacity; }  // Max number of elements

    /**
     * @brief Gets the data pointer of the queue elements.
     * 
     * Returns a constant pointer to the queue elements.
     * 
     * @return const T* Constant pointer to the queue elements.
     */
    const T *data() const { return m_data; }

private:
    T *m_data;                    /**< Pointer to the queue elements */
    int m_filled;                 /**< Current number of elements in the queue */
    int m_capacity;               /**< Maximum capacity of the queue */

    std::mutex mtx{};             /**< Mutex for thread safety */
    std::condition_variable cv{}; /**< Condition variable for synchronization */
};

#endif