#pragma once

#include "../ECS/ECS.h"
#include "../Components/ScriptComponent.h"
#include "../Components/TransformComponent.h"

class ScriptSystem : public System {
public:
    ScriptSystem()
    {
        RequireComponent<ScriptComponent>();
    }

    void CreateLuaBindings(sol::state& lua) {
        lua.new_usertype<Entity>(
            "entity",
            "get_id", &Entity::GetId,
            //"destroy", &Entity::Kill,
            //"has_tag", &Entity::HasTag,
            //"belongs_to_group", &Entity::BelongsToGroup
        );

        // Create all the bindings between C++ and Lua functions
        //lua.set_function("set_position", SetEntityPosition);
    }

    void Update() 
    {
        // Invoke the lua function of entities that have a script component
        for (auto entity : GetSystemEntities()) 
        {
            const auto script = entity.GetComponent<ScriptComponent>();
            script.func(); 
        }
    }
};
