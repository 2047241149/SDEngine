#include "LayerManager.h"

LayerManager::~LayerManager()
{
	for (auto& layer : layerArray)
	{
		layer->OnDetach();
	}

	layerArray.empty();
}

void LayerManager::PushLayer(shared_ptr<Layer> layer)
{
	layerArray.push_back(layer);
	layer->OnAttach();
}

void LayerManager::PopLayer(shared_ptr<Layer> inLayer)
{
	auto iter = std::find(layerArray.begin(), layerArray.end(), inLayer);

	if (iter != layerArray.end())
	{
		(*iter)->OnDetach();
		layerArray.erase(iter);
	}
}