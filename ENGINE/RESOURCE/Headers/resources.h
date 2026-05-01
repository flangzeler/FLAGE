//resources.h
#pragma once
#include "pch.h"

using enum resource_status;
class ResourceManager {
public:
    void Init(size_t maxTextures, size_t maxMeshes) {
        m_TexturePool = new PoolAllocator(sizeof(Texture), maxTextures, alignof(Texture));
        m_MeshPool = new PoolAllocator(sizeof(Mesh), maxMeshes, alignof(Mesh));
    }

    void Shutdown() {
        // unload all textures
        for (auto& [h, t] : m_Textures)
            t->Unload(*m_TexturePool);

        for (auto& [h, m] : m_Meshes)
            m->Unload(*m_MeshPool);

        m_Textures.clear();
        m_Meshes.clear();

        delete m_TexturePool;
        delete m_MeshPool;
    }

    // =========================================================
    // GENERIC LOAD OR GET
    // =========================================================

    template<typename T>
    std::optional<T*> LoadOrGet(
        Str handle,
        std::optional<std::string> path = std::nullopt,
        std::optional<const char*> name = std::nullopt)
    {
        if constexpr (std::is_same_v<T, Texture>) {
            return LoadOrGetImpl(handle, path, name, m_Textures, *m_TexturePool);
        }
        else if constexpr (std::is_same_v<T, Mesh>) {
            return LoadOrGetImpl(handle, path, name, m_Meshes, *m_MeshPool);
        }
    }

    template<typename T>
    void Unload(Str handle) {
        if constexpr (std::is_same_v<T, Texture>) {
            UnloadImpl(handle, m_Textures, *m_TexturePool);
        }
        else if constexpr (std::is_same_v<T, Mesh>) {
            UnloadImpl(handle, m_Meshes, *m_MeshPool);
        }
    }

    template<typename T>
    T* Get(Str handle) {
        if constexpr (std::is_same_v<T, Texture>) {
            return GetImpl(handle, m_Textures);
        }
        else if constexpr (std::is_same_v<T, Mesh>) {
            return GetImpl(handle, m_Meshes);
        }
    }

private:
    template<typename T>
    std::optional<T*> LoadOrGetImpl(
        Str handle,
        std::optional<std::string> path,
        std::optional<const char*> name,
        std::unordered_map<Str, T*>& container,
        PoolAllocator& pool)
    {
        auto it = container.find(handle);
        if (it != container.end())
            return it->second;

        if (!path.has_value() || !name.has_value())
            return std::nullopt;

        auto result = T::Load(pool, *path, *name);
        if (!result.has_value())
            return std::nullopt;

        T* resource = *result;
        container[resource->handle] = resource;

        return resource;
    }

    template<typename T>
    void UnloadImpl(
        Str handle,
        std::unordered_map<Str, T*>& container,
        PoolAllocator& pool)
    {
        auto it = container.find(handle);
        if (it == container.end()) return;

        it->second->Unload(pool);
        container.erase(it);
    }

    template<typename T>
    T* GetImpl(Str handle, std::unordered_map<Str, T*>& container) {
        auto it = container.find(handle);
        return (it != container.end()) ? it->second : nullptr;
    }

private:
    PoolAllocator* m_TexturePool = nullptr;
    PoolAllocator* m_MeshPool = nullptr;

    std::unordered_map<Str, Texture*> m_Textures;
    std::unordered_map<Str, Mesh*>    m_Meshes;
};


//auto tex  = rm.LoadOrGet<Texture>(hash("brick"), "brick.png", "brick");
//auto mesh = rm.LoadOrGet<Mesh>(hash("cube"), "cube.obj", "cube");