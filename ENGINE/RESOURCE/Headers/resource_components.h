//resource_components.h
#pragma once
#include"pch.h"

enum resource_status
{
	LOAD,
	UNLOAD,
	FAILED
};
class Texture {
public:
    resource_status status = resource_status::UNLOAD;

    Str handle = 0;
    std::string path;
    const char* name = nullptr;

    uint32_t width = 0;
    uint32_t height = 0;

public:
    static std::optional<Texture*> Load(
        PoolAllocator& pool,
        const std::string& path,
        const char* name)
    {
        void* mem = pool.Allocate();
        if (!mem) return std::nullopt;

        Texture* tex = new (mem) Texture();

        tex->handle = hash(name);
        tex->path = path;
        tex->name = name;

        // --- staging memory ---
        auto& arena = MemoryManager::GetFrameArena();
        void* temp = arena.Allocate(512 * 1024);

        bool loaded = true; // simulate

        tex->status = loaded ? resource_status::LOAD
            : resource_status::FAILED;

        arena.Reset();

        if (tex->status == resource_status::FAILED) {
            tex->~Texture();
            pool.Free(tex);
            return std::nullopt;
        }

        return tex;
    }

    void Unload(PoolAllocator& pool) {
        status = resource_status::UNLOAD;
        this->~Texture();
        pool.Free(this);
    }
};

class Mesh {
public:
    resource_status status = resource_status::UNLOAD;

    Str handle = 0;
    const char* name = nullptr;
    std::string path;

    // CPU-side data
    float* vertices = nullptr;
    uint32_t vertexCount = 0;

    uint32_t* indices = nullptr;
    uint32_t indexCount = 0;

public:
    static std::optional<Mesh*> Load(
        PoolAllocator& pool,
        const std::string& path,
        const char* name)
    {
        void* mem = pool.Allocate();
        if (!mem) return std::nullopt;

        Mesh* mesh = new (mem) Mesh();

        mesh->handle = hash(name);
        mesh->name = name;
        mesh->path = path;

        auto& arena = MemoryManager::GetFrameArena();

        // --- simulate loading ---
        mesh->vertexCount = 3;
        mesh->indexCount = 3;

        mesh->vertices = (float*)arena.Allocate(sizeof(float) * 3 * 3);
        mesh->indices = (uint32_t*)arena.Allocate(sizeof(uint32_t) * 3);

        // normally: read file → fill temp buffers → copy to permanent memory

        bool loaded = true;

        mesh->status = loaded ? resource_status::LOAD
            : resource_status::FAILED;

        arena.Reset();

        if (mesh->status == resource_status::FAILED) {
            mesh->~Mesh();
            pool.Free(mesh);
            return std::nullopt;
        }

        return mesh;
    }

    void Unload(PoolAllocator& pool) {
        status = resource_status::UNLOAD;

        // NOTE: vertices/indices are temp in this simplified version
        // later: allocate them in permanent arena or GPU memory

        this->~Mesh();
        pool.Free(this);
    }
};