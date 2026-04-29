#pragma once
#include "pch.h"
#include <cstdlib>   // malloc, free, aligned_alloc
#include <cstdint>   // uintptr_t

// ------------------------------------------------------------
// Linear Allocator
// ------------------------------------------------------------
class LinearAllocator {
public:
    LinearAllocator(size_t totalSize)
        : m_TotalSize(totalSize), m_UsedSize(0)
    {
        m_StartPtr = (uint8_t*)malloc(totalSize);
        FLAGE_ASSERT(m_StartPtr != nullptr, "Memory: Failed to allocate Linear Arena!");
    }

    ~LinearAllocator() {
        free(m_StartPtr);
        m_StartPtr = nullptr;
    }

    void* Allocate(size_t size, size_t alignment = 8) {
        uintptr_t currentPtr = reinterpret_cast<uintptr_t>(m_StartPtr + m_UsedSize);

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

    // --- Marker system (added, no API break) ---
    size_t GetMarker() const { return m_UsedSize; }

    void FreeToMarker(size_t marker) {
        FLAGE_ASSERT(marker <= m_UsedSize, "Memory: Invalid marker!");
        m_UsedSize = marker;
    }

    void Reset() { m_UsedSize = 0; }

private:
    uint8_t* m_StartPtr = nullptr;
    size_t   m_TotalSize = 0;
    size_t   m_UsedSize = 0;
};

// ------------------------------------------------------------
// Pool Allocator
// ------------------------------------------------------------
class PoolAllocator {
public:
    PoolAllocator(size_t objectSize, size_t objectCount, size_t alignment = 8)
    {
        FLAGE_ASSERT((alignment & (alignment - 1)) == 0, "Alignment must be power of two");
        FLAGE_ASSERT(alignment >= alignof(void*), "Alignment too small");

        m_Alignment = alignment;

        m_ObjSize = AlignUp(
            (objectSize < sizeof(Node)) ? sizeof(Node) : objectSize,
            alignment
        );

        m_TotalSize = AlignUp(m_ObjSize * objectCount, alignment);

        // aligned allocation (C++17)
        m_StartPtr = (uint8_t*)std::aligned_alloc(alignment, m_TotalSize);

        FLAGE_ASSERT(m_StartPtr != nullptr, "Memory: Failed to allocate Pool Arena!");
        Reset();
    }

    ~PoolAllocator() {
        free(m_StartPtr);
        m_StartPtr = nullptr;
    }

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
        if (!ptr) return;

        // Debug safety (optional but useful)
        FLAGE_ASSERT(ptr >= m_StartPtr && ptr < m_StartPtr + m_TotalSize,
                     "Memory: Invalid pointer freed to pool!");

        Node* node = (Node*)ptr;
        node->next = m_FreeList;
        m_FreeList = node;
    }

    void Reset() {
        m_FreeList = (Node*)m_StartPtr;
        Node* current = m_FreeList;

        size_t count = m_TotalSize / m_ObjSize;

        for (size_t i = 1; i < count; ++i) {
            current->next = (Node*)((uint8_t*)current + m_ObjSize);
            current = current->next;
        }

        current->next = nullptr;
    }

private:
    struct Node { Node* next; };

    static size_t AlignUp(size_t size, size_t alignment) {
        return (size + (alignment - 1)) & ~(alignment - 1);
    }

private:
    Node*    m_FreeList = nullptr;
    uint8_t* m_StartPtr = nullptr;
    size_t   m_TotalSize = 0;
    size_t   m_ObjSize = 0;
    size_t   m_Alignment = 0;
};

// ------------------------------------------------------------
// Memory Manager update
// ------------------------------------------------------------
class MemoryManager {
public:
    static inline bool Init(size_t permanentSize, size_t frameSize) {
        if (s_Initialized) return false;

        s_PermanentArena = new LinearAllocator(permanentSize);
        s_FrameArena     = new LinearAllocator(frameSize);

        LOG_INFO("MemoryManager: Systems Initialized.");
        s_Initialized = true;
        return true;
    }

    static inline void Shutdown() {
        delete s_PermanentArena;
        s_PermanentArena = nullptr;

        delete s_FrameArena;
        s_FrameArena = nullptr;

        LOG_INFO("MemoryManager: Systems Shutdown.");
    }

    static LinearAllocator& GetPermanentArena() { return *s_PermanentArena; }
    static LinearAllocator& GetFrameArena()     { return *s_FrameArena; }

    template<typename T, typename... Args>
    static T* NewPermanent(Args&&... args) {
        void* mem = s_PermanentArena->Allocate(sizeof(T), alignof(T));
        if (!mem) return nullptr;
        return new (mem) T(std::forward<Args>(args)...);
    }

private:
    static inline LinearAllocator* s_PermanentArena = nullptr;
    static inline LinearAllocator* s_FrameArena     = nullptr;
    static inline bool             s_Initialized    = false;
};