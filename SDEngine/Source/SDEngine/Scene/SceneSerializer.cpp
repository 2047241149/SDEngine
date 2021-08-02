#include "SceneSerializer.h"
#include "Actor.h"
#include <fstream>
#include <sstream>
#include "TagComponent.h"
#include "TransformComponent.h"
#include "MeshComponent.h"
using std::ofstream;
using std::ifstream;
using std::stringstream;

YAML::Emitter& operator<<(YAML::Emitter & out, XMFLOAT3 value)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << value.x << value.y << value.z << YAML::EndSeq;
	return out;
}

namespace YAML
{
	template<>
	struct convert<XMFLOAT3>
	{
		static Node encode(const XMFLOAT3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, XMFLOAT3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
			{
				return false;
			}

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};
}

SceneSerializer::SceneSerializer(Scene* inScene)
	:scene(inScene)
{
}

void SceneSerializer::Serialize(const string& fileName)
{
	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "Map" << YAML::Value << "Untitled";
	out << YAML::Key << "Actor" << YAML::Value << YAML::BeginSeq;
	
	scene->GetResgtry().each([&](auto entityId)
	{
		Actor actor(entityId, scene);
		if (!actor.IsValid())
			return;

		SerializeActor(out, actor);
	});

	out << YAML::EndSeq;
	out << YAML::EndMap;
	ofstream fout(fileName.c_str());
	fout << out.c_str();

	Log::Info("Save Map {0}", fileName);
}

void SceneSerializer::SerializeActor(YAML::Emitter& out, Actor& actor)
{
	out << YAML::BeginMap;
	out << YAML::Key << "Actor" << YAML::Value << "Untitled";

	if (actor.HasComponent<TagComponent>())
	{
		out << YAML::Key << "TagComponent";
		out << YAML::BeginMap;

		auto& tag = actor.GetComponent<TagComponent>().tag;
		out << YAML::Key << "Tag" << YAML::Value << tag;
		out << YAML::EndMap;
	}

	if (actor.HasComponent<TransformComponent>())
	{
		out << YAML::Key << "TransformComponent";
		out << YAML::BeginMap;

		auto& transformCpt = actor.GetComponent<TransformComponent>();
		out << YAML::Key << "Position" << YAML::Value << transformCpt.position;
		out << YAML::Key << "Rotation" << YAML::Value << transformCpt.rotation;
		out << YAML::Key << "Scale" << YAML::Value << transformCpt.scale;
		out << YAML::EndMap;
	}

	if (actor.HasComponent<MeshComponent>())
	{
		out << YAML::Key << "MeshComponent";
		out << YAML::BeginMap;

		auto& meshCpt = actor.GetComponent<MeshComponent>();
		out << YAML::Key << "MeshFile" << YAML::Value << meshCpt.meshFileName;
		out << YAML::EndMap;
	}

	out << YAML::EndMap;
}

bool SceneSerializer::Deserialize(const string& fileName)
{
	ifstream is(fileName);
	stringstream strStream;
	strStream << is.rdbuf();

	YAML::Node data = YAML::Load(strStream.str());
	if (!data["Map"])
		return false;

	const string mapName = data["Map"].as<string>();
	auto& actorDatas = data["Actor"];
	if (actorDatas)
	{
		for (auto& actorData : actorDatas)
		{
			string name;

			//TagComponent
			auto& tagData = actorData["TagComponent"];
			if (tagData)
				name = tagData["Tag"].as<string>();

			Actor& actor = scene->CreateActor(name);

			//Transform Component
			auto& transformData = actorData["TransformComponent"];
			if (transformData)
			{
				auto& transformCpt = actor.GetOrAddComponent<TransformComponent>();
				transformCpt.position = transformData["Position"].as<XMFLOAT3>();
				transformCpt.rotation = transformData["Rotation"].as<XMFLOAT3>();
				transformCpt.scale = transformData["Scale"].as<XMFLOAT3>();
			}

			//MeshComponent
			auto& meshData = actorData["MeshComponent"];
			if (meshData)
			{
				auto& meshCpt = actor.GetOrAddComponent<MeshComponent>();
				const string& meshFile = meshData["MeshFile"].as<string>();
				meshCpt.SetMesh(meshFile);
			}
		}
	}

	return true;
}