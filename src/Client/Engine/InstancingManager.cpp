#include "pch.h"
#include "InstancingManager.h"
#include "InstancingBuffer.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "Camera.h"
#include "Animator.h"

void InstancingManager::Render(vector<shared_ptr<CGameObject>>& gameObjects)
{
	map<uint64, vector<shared_ptr<CGameObject>>> cache;

	for (shared_ptr<CGameObject>& gameObject : gameObjects)
	{
		const uint64 instanceId = gameObject->GetMeshRenderer()->GetInstanceID();
		cache[instanceId].push_back(gameObject);
	}

	for (auto& [instanceId, vec] : cache)
	{
		//const vector<shared_ptr<CGameObject>>& vec = value;

		if (vec[0]->GetAnimator() != nullptr)
		{
			if (vec[0]->GetAnimator()->GetPlayInstance() == false)
			{
				for (const shared_ptr<CGameObject>& gameObject : vec)
				{
					gameObject->GetMeshRenderer()->Render();
				}
			}
		}
		else 
		{
			if (vec.size() == 1)
			{
				vec[0]->GetMeshRenderer()->Render();
			}
			else
			{
				//const uint64 instanceId = key;

				for (const shared_ptr<CGameObject>& gameObject : vec)
				{
					InstancingParams params;
					params.matWorld = gameObject->GetTransform()->GetLocalToWorldMatrix();
					params.matWV = params.matWorld * Camera::S_MatView;
					params.matWVP = params.matWorld * Camera::S_MatView * Camera::S_MatProjection;

					AddParam(instanceId, params);
				}

				shared_ptr<InstancingBuffer>& buffer = m_buffers[instanceId];
				vec[0]->GetMeshRenderer()->Render(buffer);
			}
		}
	}
}

void InstancingManager::ClearBuffer()
{
	for (auto& pair : m_buffers)
	{
		shared_ptr<InstancingBuffer>& buffer = pair.second;
		buffer->Clear();
	}
}

void InstancingManager::AddParam(uint64 instanceId, InstancingParams& data)
{
	if (m_buffers.find(instanceId) == m_buffers.end())
		m_buffers[instanceId] = make_shared<InstancingBuffer>();

	m_buffers[instanceId]->AddData(data);
}