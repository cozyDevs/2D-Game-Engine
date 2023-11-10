#include "ECS.h"
#include "../Logger/Logger.h"

int IComponent::nextId = 0;

int Entity::GetId() const 
{
	return id;
}


void System::AddEntityToSystem(Entity entity)
{
	entities.push_back(entity);
}

// Use of lambda function to iterate through the entities and find the entity to remove
void System::RemoveEntityFromSystem(Entity entity)
{
	entities.erase(std::remove_if (entities.begin(), entities.end(), [&entity](Entity other)
		{
			return entity == other;
		}), entities.end());
}

std::vector<Entity> System::GetSystemEntities() const
{
	return entities;
}

const Signature& System::GetComponentSignature() const
{
	return componentSignature;
}

Entity Registry::CreateEntity()
{
	int entityId;
	entityId = numEntities++;

	// Creating the entity and pointing to its parent registry
	Entity entity(entityId);
	entity.registry = this;
	entitiesToBeAdded.insert(entity);

	// Assert that entityComponentVector is the correct size for entity being created, resize if necessary
	if (entityId >= entityComponentSignatures.size())
	{
		entityComponentSignatures.resize(entityId + 1);
	}

	Logger::Log("Entity created with id: " + std::to_string(entityId));
	return entity;
}

// Match the component signatures of the entity and the system 
void Registry::AddEntityToSystem(Entity entity)
{
	const auto entityId = entity.GetId();

	const auto& entityComponentSignature = entityComponentSignatures[entityId];

	// Loop through all of the systems
	for (auto& system : systems)
	{
		const auto& systemComponentSignature = system.second->GetComponentSignature();

		// Bitwise comparison between system component signature and entity component signature
		bool isInterested = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;

		if (isInterested)
		{
			system.second->AddEntityToSystem(entity);
		}
	}
}

void Registry::Update()
{
	// Adds the entities that are waiting to be created
	for (auto entity : entitiesToBeAdded)
	{
		AddEntityToSystem(entity);
	}

	// Clear the entity set since it might change next frame
	entitiesToBeAdded.clear();
}