# Multi-threaded Element Queue Project (C++)
Develop a class from scratch to queue a finite number of primitive types (e.g., int).

The class interface looks like this: 

```c++
    class Queue<T> 
    {
        Queue(int size) {...}
        void Push(T element) {...}
        T Pop() {...}
        T PopWithTimeout(int milliseconds) {...}
        int Count() {...} // Amount of elements stored now
        int Size() {...} // Max number of elements
    }
```