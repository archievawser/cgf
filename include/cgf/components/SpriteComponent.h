#pragma once

#include <vector>

#include "core/Memory.h"
#include "core/Actor.h"
#include "core/Component.h"
#include "core/AssetLibrary.h"
#include "core/Common.h"

#include "graphics/Texture.h"

#include "components/DynamicMeshComponent.h"


struct SpriteState
{
	SpriteState(int tileIndex = 0, glm::vec2 position = glm::vec2(0), glm::vec2 size = glm::vec2(100))
		: m_TileIndex(tileIndex), m_Position(position), m_Size(size)
	{
		
	}

	FORCEINLINE void SetPosition(glm::vec2 position)
	{
		m_Position = position;
	}

	FORCEINLINE glm::vec2 GetPosition() const
	{
		return m_Position;
	}

	FORCEINLINE void SetSize(glm::vec2 size)
	{
		m_Size = size;
	}

	FORCEINLINE glm::vec2 GetSize() const
	{
		return m_Size;
	}

	FORCEINLINE void SetTileIndex(int tileIndex)
	{
		m_TileIndex = tileIndex;
	}

	FORCEINLINE int GetTileIndex() const
	{
		return m_TileIndex;
	}
	
	bool Mutated = true;

private:
	glm::vec2 m_Position;
	glm::vec2 m_Size;
	int m_TileIndex;
};


class SpriteBatchComponent : public DynamicMeshComponent
{
public:
	SpriteBatchComponent() = default;

	void Start() override
	{
		m_SpriteMaterial = Game->GetAssetLibrary()->Get<MaterialInstance>("Sprite");
		SetMaterial(m_SpriteMaterial);

		m_Atlas = Game->GetAssetLibrary()->Get<Texture2D>("Atlas");
	}

	void TickComponent(double dT)
	{
		m_Vertices.clear();
		m_Indices.clear();

		for(int i = 0; i < m_SpriteStatePool.GetCount(); i++)
		{
			SpriteState& spriteState = m_SpriteStatePool[i];

			unsigned int firstIndex = i * 4;

			unsigned int quadIndices[]
			{
				firstIndex + 2, firstIndex + 1, firstIndex + 0,
				firstIndex + 0, firstIndex + 3, firstIndex + 2
			};

			glm::vec2 halfSize = spriteState.GetSize() / 2.0f;

			Vertex quadVertices[] 
			{
				{
					.Position = glm::vec3(spriteState.GetPosition(), 0) + glm::vec3(-halfSize.x, -halfSize.y, 0),
					.Normal = glm::vec3(0, 0, 0),
					.UV0 = glm::vec3(0, 0, 0),
				},
				{
					.Position = glm::vec3(spriteState.GetPosition(), 0) + glm::vec3(halfSize.x, -halfSize.y, 0),
					.Normal = glm::vec3(0, 0, 0),
					.UV0 = glm::vec3(1, 0, 0),
				},
				{
					.Position = glm::vec3(spriteState.GetPosition(), 0) + glm::vec3(halfSize.x, halfSize.y, 0),
					.Normal = glm::vec3(0, 0, 0),
					.UV0 = glm::vec3(1, 1, 0),
				},
				{
					.Position = glm::vec3(spriteState.GetPosition(), 0) + glm::vec3(-halfSize.x, halfSize.y, 0),
					.Normal = glm::vec3(0, 0, 0),
					.UV0 = glm::vec3(0, 1, 0),
				},
			};

			m_Vertices.insert(m_Vertices.end(), std::begin(quadVertices), std::end(quadVertices));
			m_Indices.insert(m_Indices.end(), std::begin(quadIndices), std::end(quadIndices));
		}

		Upload();
	}

	PooledPtr<SpriteState> CreateSprite()
	{
		return m_SpriteStatePool.Create();
	}

private:
	void Upload()
	{
		SetVertexData(m_Vertices.data(), sizeof(Vertex), m_Vertices.size());
		SetIndexData(m_Indices.data(), m_Indices.size());
	}

	std::vector<Vertex> m_Vertices;
	std::vector<unsigned int> m_Indices;
	Pool<SpriteState> m_SpriteStatePool;
	SharedPtr<MaterialInstance> m_SpriteMaterial;
	SharedPtr<Texture2D> m_Atlas;
};