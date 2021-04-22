#include <iostream>
#include <windows.h>
#include <process.h>
#include <string>

using mutex_t = int;

void lock_mutex(mutex_t* mx)
{
    __asm
    {
    try_lock:
        mov edx, 1          ; load constant into reg
        mov ecx, [mx]       ; load address of mx into reg
        xchg [ecx], edx     ; atomically exchange values of 
                            ; dereferenced variable and the 
                            ; registry containing 1
        cmp [ecx], edx      ; check if value has changed
        jne finish          ; finish proc if mx is changed to 1
                            ; (mutex locked)
        call SwitchToThread ; yield to another thread
                            ; and give another try after jmp
        jmp try_lock
    finish:
    }
}

void unlock_mutex(mutex_t* mx)
{
    __asm
    {
        mov edx, [mx]       ; load var address into edx
        mov [edx], 0        ; set mutex variable to 0
    }
}

struct ThreadData
{
    mutex_t myMutex = 0;
    std::string data;
};

unsigned __stdcall ThreadFunc(void* threadData)
{
    _endthreadex(0);
    return 0;
}

int main()
{
    struct ThreadData threadData;
    constexpr int numberOfThreadsToCreate = 4;
    uintptr_t threads[numberOfThreadsToCreate];

    memset(threads, 0, sizeof(uintptr_t) * numberOfThreadsToCreate);

    for (int threadNumber = 0; threadNumber < numberOfThreadsToCreate; ++threadNumber)
    {
        threads[threadNumber] =
            _beginthreadex(nullptr,
                0,
                ThreadFunc,
                static_cast<void*>(&threadData),
                0,
                nullptr);
    }



    return 0;
}
