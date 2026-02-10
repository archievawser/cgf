#pragma once

#include <string>
#include <filesystem>
#include <unordered_map>
#include <fstream>

#include "core/Memory.h"
#include "graphics/Shader.h"
#include "graphics/Material.h"

#include "assimp/Importer.hpp" 
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "glm/glm.hpp"

#include "buildtool/Assets.h"


/**
 * @brief Provides streamed access to assets included in a project
 * 
 * CGF's build tool compiles a project's runtime dependencies into a CGFB (C++ Game Framework Binary).
 * The AssetLibrary class streams the compiled CGFB and provides an interface to convert arbitrary
 * asset data into an instance of an asset. The asset data pertaining to a named asset is loaded
 * via AssetDataLoader upon a corresponding call to Get().
 **/
class AssetLibrary
{
public:
	AssetLibrary(const char* projectFilePath);

	/**
	 * @tparam AssetType 
	 * @param assetName The name of the asset as registered in the project descriptor; not its file path
	 * @return AssetType The requested asset
	 */
	template<typename AssetT>
	SharedPtr<AssetT> Get(std::string assetName)
	{
		static std::unordered_map<std::string, SharedPtr<AssetT>> loadedAssets;

		if(loadedAssets.find(assetName) != loadedAssets.end())
			return loadedAssets[assetName];

		return loadedAssets[assetName] = Load<AssetT>(assetName);
	}

	template<typename AssetT>
	SharedPtr<AssetT> Load(std::string assetName)
	{
		throw "Asset type not recognized";
	}

private:
	btools::AssetDataLoader m_AssetDataLoader;
	char* m_AssetData;
	int m_AssetDataSize;
};


template<>
inline SharedPtr<Material> AssetLibrary::Load(std::string materialName)
{
	char* rawSource;
	int rawSourceLength;
	bool success = m_AssetDataLoader.Load(materialName.c_str(), &rawSource, &rawSourceLength);

	CGF_ASSERT(success, "Failed to load " + materialName);

	std::string source (rawSource, rawSourceLength);

	auto fs = std::make_shared<Shader>(materialName, source, SHADER_TYPE_PIXEL);
	auto vs = std::make_shared<Shader>(materialName, source, SHADER_TYPE_VERTEX);

	return SharedPtr<Material>::CreateTraced(materialName + "_Material", vs, fs);
}


template<>
inline SharedPtr<MaterialInstance> AssetLibrary::Load(std::string materialName)
{
	return SharedPtr<MaterialInstance>::Create(Get<Material>(materialName));
}


template<>
inline SharedPtr<std::string> AssetLibrary::Load(std::string materialName)
{
	char* rawSource;
	int rawSourceLength;
	bool success = m_AssetDataLoader.Load(materialName.c_str(), &rawSource, &rawSourceLength);

	CGF_ASSERT(success, "Failed to load " + materialName);

	std::string source (rawSource, rawSourceLength);
	return SharedPtr<std::string>::CreateTraced(materialName + "_Source", std::move(source));
}


struct StaticMesh
{
	StaticMesh(unsigned int indexCount, RefCntAutoPtr<IBuffer> indexBuffer, RefCntAutoPtr<IBuffer> vertexBuffer)
		: IndexCount(indexCount), IndexBuffer(indexBuffer), VertexBuffer(vertexBuffer)
	{

	}

	unsigned int IndexCount;
	RefCntAutoPtr<IBuffer> IndexBuffer;
	RefCntAutoPtr<IBuffer> VertexBuffer;
};


struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 UV0;
};


template<>
inline SharedPtr<StaticMesh> AssetLibrary::Load(std::string materialName)
{
	char* rawSource;
	int rawSourceLength;
	bool success = m_AssetDataLoader.Load(materialName.c_str(), &rawSource, &rawSourceLength);

	CGF_ASSERT(success, "Failed to load " + materialName);

	Assimp::Importer importer;

	const aiScene *scene = importer.ReadFileFromMemory(rawSource, rawSourceLength, 
		aiProcess_CalcTangentSpace | 
		aiProcess_Triangulate | 
		aiProcess_JoinIdenticalVertices, 
		"fbx");

	CGF_ASSERT(scene->mNumMeshes, "No mesh found");

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	aiMesh *mesh = scene->mMeshes[0];

	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		indices.insert(indices.end(), mesh->mFaces[i].mIndices, mesh->mFaces[i].mIndices + mesh->mFaces[i].mNumIndices);
	}

	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex newVertex;

		auto pos = mesh->mVertices[i];
		auto nrm = mesh->mNormals[i];
		auto uv0 = mesh->mTextureCoords[0][i];

		newVertex.Position = glm::vec3(pos.x, pos.y, pos.z);
		newVertex.Normal = glm::vec3(nrm.x, nrm.y, nrm.z);
		newVertex.UV0 = glm::vec3(uv0.x, uv0.y, uv0.z);

		vertices.push_back(newVertex);
	}

	RefCntAutoPtr<IBuffer> vbuffer;
	BufferData vdata(vertices.data(), vertices.size() * sizeof(Vertex));
	BufferDesc vbufferDesc;
	vbufferDesc.Name = "test";
	vbufferDesc.Size = vertices.size() * sizeof(Vertex);
	vbufferDesc.Usage = USAGE_IMMUTABLE;
	vbufferDesc.BindFlags = BIND_VERTEX_BUFFER;
	Game->GetGraphicsContext()->GetRenderDevice()->CreateBuffer(vbufferDesc, &vdata, &vbuffer);

	RefCntAutoPtr<IBuffer> ibuffer;
	BufferData idata(indices.data(), indices.size() * sizeof(unsigned int));
	BufferDesc ibufferDesc;
	ibufferDesc.Name = "test2";
	ibufferDesc.Size = indices.size() * sizeof(unsigned int);
	ibufferDesc.Usage = USAGE_IMMUTABLE;
	ibufferDesc.BindFlags = BIND_INDEX_BUFFER;
	Game->GetGraphicsContext()->GetRenderDevice()->CreateBuffer(ibufferDesc, &idata, &ibuffer);

	return SharedPtr<StaticMesh>::CreateTraced(materialName + "_Source", indices.size(), ibuffer, vbuffer);
}