#pragma once
#include "pch.h"



class LinearAllocator {
public:
    LinearAllocator(size_t totalSize) : m_TotalSize(totalSize), m_UsedSize(0) {
        m_StartPtr = (uint8_t*)malloc(totalSize);
        FLAGE_ASSERT(m_StartPtr != nullptr, "Memory: Failed to allocate Linear Arena!");
    }

    ~LinearAllocator() { free(m_StartPtr); }

    void* Allocate(size_t size, size_t alignment = 8) {
        uintptr_t currentPtr = (uintptr_t)m_StartPtr + m_UsedSize;
        size_t padding = (alignment - (currentPtr % alignment)) % alignment;

        if (m_UsedSize + padding + size > m_TotalSize) {
            FLAGE_ASSERT(false, "Memory: Linear Allocator Overflow!");
            return nullptr;
        }

        m_UsedSize += padding;
        void* ptr = (void*)(m_StartPtr + m_UsedSize);
        m_UsedSize += size;
        return ptr;
    }

    void Reset() { m_UsedSize = 0; }

private:
    uint8_t* m_StartPtr = nullptr;
    size_t   m_TotalSize = 0;
    size_t   m_UsedSize = 0;
};

class PoolAllocator {
public:
    PoolAllocator(size_t objectSize, size_t objectCount, size_t alignment = 8) {
        m_ObjSize = (objectSize < sizeof(Node)) ? sizeof(Node) : objectSize;
        m_TotalSize = m_ObjSize * objectCount;
        m_StartPtr = (uint8_t*)malloc(m_TotalSize);

        FLAGE_ASSERT(m_StartPtr != nullptr, "Memory: Failed to allocate Pool Arena!");
        Reset();
    }

    ~PoolAllocator() { free(m_StartPtr); }

    void* Allocate() {
        if (m_FreeList == nullptr) {
            FLAGE_ASSERT(false, "Memory: Pool Allocator is full!");
            return nullptr;
        }
        Node* node = m_FreeList;
        m_FreeList = m_FreeList->next;
        return (void*)node;
    }

    void Free(void* ptr) {
        Node* node = (Node*)ptr;
        node->next = m_FreeList;
        m_FreeList = node;
    }

    void Reset() {
        m_FreeList = (Node*)m_StartPtr;
        Node* current = m_FreeList;
        for (size_t i = 1; i < (m_TotalSize / m_ObjSize); ++i) {
            current->next = (Node*)((uint8_t*)current + m_ObjSize);
            current = current->next;
        }
        current->next = nullptr;
    }

private:
    struct Node { Node* next; };
    Node* m_FreeList = nullptr;
    uint8_t* m_StartPtr = nullptr;
    size_t   m_TotalSize = 0;
    size_t   m_ObjSize = 0;
};

class MemoryManager {
public:
    static inline bool Init(size_t permanentSize, size_t frameSize) {
        if (s_Initialized) return false;

        s_PermanentArena = new LinearAllocator(permanentSize);
        s_FrameArena = new LinearAllocator(frameSize);

        LOG_INFO("MemoryManager: Systems Initialized.");
        s_Initialized = true;
        return true;
    }

    static inline void Shutdown() {
        delete s_PermanentArena;
        delete s_FrameArena;
        LOG_INFO("MemoryManager: Systems Shutdown.");
    }

    static LinearAllocator& GetPermanentArena() { return *s_PermanentArena; }
    static LinearAllocator& GetFrameArena() { return *s_FrameArena; }

    template<typename T, typename... Args>
    static T* NewPermanent(Args&&... args) {
        void* mem = s_PermanentArena->Allocate(sizeof(T), alignof(T));
        return new (mem) T(std::forward<Args>(args)...);
    }

private:
    static inline LinearAllocator* s_PermanentArena = nullptr;
    static inline LinearAllocator* s_FrameArena = nullptr;
    static inline bool             s_Initialized = false;
};