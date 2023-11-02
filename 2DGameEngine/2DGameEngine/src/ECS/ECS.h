#pragma once

#include <bitset>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <set>
#include <memory>

// Max amount of components for a single entity 
const unsigned int MAX_COMPONENTS = 32;

// Component signature represented as bits to track what components an entity has
typedef std::bitset<MAX_COMPONENTS> Signature;


// Component has one id for each component type
struct IComponent
{
	protected: 
		static int nextId;
};

// Component is a template class that inherits from the BaseComponent struct
template <typename T> 
class Component : public IComponent
{
	// Getter for the unique id of component T
	static int GetId()
	{
		static auto id = nextId++;
		return id;
	}

};


// Entity class with private id attribute and getter method for the Id. Along with constructor
// and constructor initialization list
class Entity
{
	private:
		int id;

	public:
		Entity(int id) : id(id) {};
		Entity(const Entity& entity) = default;
		int GetId() const;

		// Overload operators for the entity class
		Entity& operator = (const Entity& other) = default;
		bool operator == (const Entity& other) const { return id == other.id; }
		bool operator != (const Entity& other) const { return id != other.id; }
		bool operator >(const Entity& other) const { return id > other.id; }
		bool operator <(const Entity& other) const { return id < other.id; }

};

// System will process the entities based on the matching components in the component signature
class System
{
	private: 
		Signature componentSignature;
		std::vector<Entity> entities;

	public:
		System() = default;
		~System() = default;

		void AddEntityToSystem(Entity entity);
		void RemoveEntityFromSystem(Entity entity);
		std::vector<Entity> GetSystemEntities() const;
		const Signature& GetComponentSignature() const;

		// Has the component type from the signature so the system knows which components the entity has
		template <typename TComponent> void RequireComponent();


};

// Base class for template class pool
class IPool
{
	public:
		virtual ~IPool() {}
};

// Pool class represents data of type T as a template class, essentially wrapper around
// a vector class
template <typename T>
class Pool : public IPool
{
	private:
		std::vector<T> data;

	public:
		Pool(int size = 100)
		{
			data.resize(size);
		}

		virtual ~Pool() = default;

		bool isEmpty() const
		{
			return data.empty();
		}

		int GetSize() const
		{
			return data.size();
		}

		void SetSize(int newSize)
		{
			data.resize(newSize);
		}

		void Clear()
		{
			data.clear();
		}
		
		void Add(T object)
		{
			data.push_back(object);
		}

		void Set(int index, T object)
		{
			data[index] = object;
		}

		T& Get(int index)
		{
			return static_cast<T&>(data[index]);
		}

		T& operator [](unsigned int index)
		{
			return data[index];
		}
};




// Manages all the entities, components, and systems including creation and destruction of each
class Registry
{
	private:
		int numEntities = 0;

		// Each index of the pool represents a different component, each index of that pool represents
		// a different entity 
		std::vector<IPool*> componentPools;

		// The signature allows us to know which components are on an entity
		std::vector<Signature> entityComponentSignatures;

		std::unordered_map<std::type_index, System*> systems;

		std::set<Entity> entitiesToBeAdded;
		std::set<Entity> entitiesToBeKilled;

	public:
		Registry() = default;

		void Update();
		
		Entity CreateEntity();

		// Component management functions
		template <typename T, typename ...TArgs> void AddComponent(Entity entity, TArgs&& ...args);
		template <typename T> void RemoveComponent(Entity entity);
		template <typename T> bool HasComponent(Entity entity) const;

		// System management functions
		template <typename TSystem, typename ...TArgs> void AddSystem(TArgs&& ...args);
		template <typename TSystem> void RemoveSystem();
		template <typename TSystem> bool HasSystem() const;
		template <typename TSystem> TSystem& GetSystem() const;

		// Entity is added to the systems based on its component signature
		void AddEntityToSystem(Entity entity);
};


// Implementation of template functions
template <typename TComponent> 
void System::RequireComponent() 
{
	const auto componentId = Component<TComponent>::GetId();
	componentSignature.set(componentId);
}


// Create new system while creating new key and value pair
template <typename TSystem, typename ...TArgs> 
void Registry::AddSystem(TArgs&& ...args)
{
	TSystem* newSystem(new TSystem(std::forward<TArgs>(args)...));
	systems.insert(std::make_pair(std::type_index(typeid(TSystem)), newSystem));
}


// Remove a system from the systems map
template <typename TSystem> 
void Registry::RemoveSystem()
{
	auto system = systems.find(std::type_index(typeid(TSystem)));
	systems.erase(system);
}


template <typename TSystem> 
bool Registry::HasSystem() const
{
	return systems.find(std::type_index(typeid(TSystem))) != systems.end();
}

template <typename TSystem> 
TSystem& Registry::GetSystem() const
{
	auto system = systems.find(std::type_index(typeid(TSystem)));
	return *(std::static_pointer_cast<TSystem>(system->second));
}


template <typename T, typename ...TArgs>
void Registry::AddComponent(Entity entity, TArgs&& ...args)
{
	const auto componentId = Component<T>::GetId();
	const auto entityId = entity.GetId();

	if (componentId >= componentPools.size())
	{
		// Resizing the vector if the component id is greater than the current size of the vector
		componentPools.resize(componentId + 1, nullptr);
	}

	// Creating the component pool for that type if it doesn't already exist
	if (!componentPools[componentId])
	{
		Pool<T>* newComponentPool = new Pool<T>();
		componentPools[componentId] = newComponentPool;
	}

	Pool<T>* componentPool = Pool<T>(componentPools[componentId]);

	// Increasing the size of the component pool if the entity id is larger than its current size
	if (entityId >= componentPool->GetSize())
	{
		componentPool->SetSize(numEntities);
	}

	// Create new component and forward the parameters
	T newComponent(std::forward<TArgs>(args)...);

	// Add the new component to the component pool
	componentPool->Set(entityId, newComponent);

	// Set the component id of the bitset to 1
	entityComponentSignatures[entityId].set(componentId);
}

// To remove component, the component index in the bitset will be set to false
template <typename T>
void Registry::RemoveComponent(Entity entity)
{
	const auto componentId = Component<T>::GetId();
	const auto entityId = entity.GetId();

	entityComponentSignatures[entityId].set(componentId, false);
}

// Returns true if the component is set in the specified entity
template <typename T>
bool Registry::HasComponent(Entity entity) const
{
	const auto componentId = Component<T>::GetId();
	const auto entityId = entity.GetId();

	return entityComponentSignatures[entityId].test(componentId);
}





