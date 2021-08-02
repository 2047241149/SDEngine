#pragma once
#include "Scene.h"
#include "yaml-cpp/yaml.h"

class SceneSerializer
{
public:
	SceneSerializer(Scene* inScene);

	void Serialize(const string& fileName);
	void SerializeActor(YAML::Emitter& out, Actor& actor);
	bool Deserialize(const string& fileName);

private:
	Scene* scene;
};