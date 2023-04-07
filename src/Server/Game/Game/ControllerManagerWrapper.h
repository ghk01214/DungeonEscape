#pragma once
#include "physx_utils.h"

class ControllerWrapper;

class ControllerManagerWrapper
{
public:
	void Init();
	void CreateController();
	void UpdateControllers();
public:
	physx::PxControllerManager* GetControllerManager();

private:
	physx::PxControllerManager*		m_manager = nullptr;
	std::vector<ControllerWrapper*> m_controllerWrappers;
	physx::PxScene*					m_scene = nullptr;
};


