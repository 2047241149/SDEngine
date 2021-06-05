#pragma once
#include "Layer.h"

class SD_API LayerManager
{
private:
	vector<shared_ptr<Layer>> layerArray;

public:
	LayerManager() = default;
	~LayerManager();

public:
	void PushLayer(shared_ptr<Layer> layer);
	void PopLayer(shared_ptr<Layer> layer);

public:
	vector<shared_ptr<Layer>>::iterator Begin() { return layerArray.begin(); }
	vector<shared_ptr<Layer>>::iterator End() { return layerArray.end(); }
};