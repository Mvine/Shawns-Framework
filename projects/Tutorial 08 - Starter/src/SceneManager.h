#pragma once

#include "Scene.h"
#include <unordered_map>

class SceneManager {
public:
	struct SceneIterator {
		typedef std::unordered_map<std::string, Scene*>::iterator iterator;
		typedef std::unordered_map<std::string, Scene*>::const_iterator const_iterator;

		iterator begin() { return SceneManager::_KnownScenes.begin(); }
		iterator end() { return SceneManager::_KnownScenes.end(); }

		const_iterator begin() const { return SceneManager::_KnownScenes.begin(); }
		const_iterator end() const { return SceneManager::_KnownScenes.end(); }
	};

	static Scene* Current();

	static bool SetCurrentScene(const std::string& name);
	static bool HasScene(const std::string& name);

	static Scene* Get(const std::string& name);
	
	/*
		Registers a new scene to the scene manager, with the given name. If a scene pointer is not given, a new scene will be created
		@param name The name of the scene
		@param scene The scene to register, or nullptr to create a new scene
	*/
	static void RegisterScene(const std::string& name, Scene* scene = nullptr);

	/*
		Returns an iterator for iterating over all of the scenes in the scene manager
		ex:
		    auto& it = SceneManager::Each()
			for(auto& pair : it)
			   . . .
	*/
	static SceneIterator Each();

	static void DestroyScenes();

	static entt::registry Prefabs;

private:
	static Scene* _CurrentScene;
	static std::unordered_map<std::string, Scene*> _KnownScenes;
};

// We can make some macros to shorten our calls
#define CurrentScene() SceneManager::Current()
#define CurrentRegistry() SceneManager::Current()->Registry()
#define GetRegistry(name) SceneManager::Get(name)->Registry()