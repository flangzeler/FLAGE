#pragma once
#include"pch.h"


	 enum class Light_type {Directional , Point , Sun , Area};
	 enum class System_type {Path , NavMesh /*....*/ };
	 enum class Shape_type {Cube , Sphere ,/*...*/  Mesh};

struct Entity
{
	uint32_t  id;

	  //...
};

struct c_name
{
	std::string name;
};
struct c_mesh
{
	Shape_type type;
	uint32_t Handel;
	//...
};
struct c_logic
{

	//...
};
struct c_system
{
	System_type type;
	//,,,
};
struct c_render
{

	//...
};
struct c_transform
{

 //...
};
enum ComponentBit
{
	BIT_NAME = 0,
	BIT_MESH = 1,
	BIT_LOGIC = 2,
	BIT_SYSTEM = 3,
	BIT_RENDER = 4,
	BIT_TRANSFORM = 5
};

class entt 
{
			 
	entt()
	{
		entities.reserve(MAX_ENTITIES);
		m_signature.reserve(MAX_ENTITIES);
	}
	Entity create_enTT()
	{
		Entity e{ entt::next_id++ };
		FLAGE_ASSERT(entities.size() < MAX_ENTITIES, "ECS: Out of Entity slots!");
		entities.push_back(e);
		if (e.id <= m_signature.size())
		{
			m_signature.push_back(std::bitset<32>());
			name.push_back     ({});
			mesh.push_back     ({});
			logic.push_back    ({});
			system.push_back   ({});
			render.push_back   ({});
			transform.push_back({});
		}
		return e;
	}
	Entity* find_enTT(uint32_t id)
	{
		for (auto& e : entities)
		{
			if (e.id == id) return &e;
		}	
		return nullptr;

	}

	//also make some get_component functions in future

	void add_name() {}	           //<=...	void add_transform(uint32_t entity_id, c_transform data) {m_transforms[entity_id] =data;  "m_signatures[entity_id].set(BIT_TRANSFORM); }
	void add_mesh      () {}	   //<=...
	void add_logic     () {}	   //<=...
	void add_system    () {}	   //<=...
	void add_render    () {}	   //<=...
	void add_transform () {}	   //<=...


	template<typename T>
	bool has(Entity e, T t) const { return t.find(e.id) != t.end(); } //add for all the components separately in future	 like as follow:

	//bool has_transform(Entity e) const {
		
	//	return m_signatures[e.id].test(ComponentBit::Transform);
	//}

	//bool has_mesh(Entity e) const {
		//return m_signatures[e.id].test(ComponentBit::Mesh);
	//}
	
	static constexpr size_t MAX_ENTITIES = 10000;
	static uint32_t next_id;
	std::vector<std::bitset<32>> m_signature;
	std::vector<Entity>          entities;
	std::vector<c_name>          name;
	std::vector<c_mesh>          mesh;
	std::vector<c_logic>         logic;
	std::vector<c_system>        system;
	std::vector<c_render>        render;
	std::vector<c_transform>     transform;
};



