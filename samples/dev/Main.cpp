#include "cgf.h"

#include <memory>

#include "core/Memory.h"
#include "core/Events.h"

#include "assimp/Importer.hpp" 
#include "assimp/scene.h"
#include "assimp/postprocess.h"


struct RuntimeTypeInfo
{
	static int TypeCount;

	static int NextTypeIndex()
	{
		static int value = 0;
		return value++;
	}

	template<typename T>
	static int GetTypeIndex()
	{
		static int i = (TypeCount++, NextTypeIndex());

		return i;
	}

	template<typename... T>
	static int GetTypeHash()
	{
		int out = 0;
		
		((out |= 1 << GetTypeIndex<T>()), ...);

		return out;
	}
};


class GameDerivative : public GameBase
{
public:
	void Start() override
	{
		auto baseMaterial = GetAssetLibrary()->Load<Material>("Red");
		auto material1 = SharedPtr<MaterialInstance>::Create(baseMaterial);

		baseMaterial = GetAssetLibrary()->Load<Material>("Green");
		auto material2 = SharedPtr<MaterialInstance>::Create(baseMaterial);

		glm::mat4 proj = glm::perspectiveFov(70.f, 1920.f, 1080.f, 0.1f, 1000.f);
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -10));
		glm::mat4 model = glm::rotate(glm::mat4x4(1.0f), -0.f, glm::vec3(1.0f, 0.f, 0.f));

		glm::mat4x4 mvp = proj * view * model;

		DeviceVarBinding<glm::mat4> binding1 = material1->CreateVertexVariableBinding<glm::mat4>("WorldData");
		binding1.Set(mvp);

		DeviceVarBinding<glm::mat4> binding2 = material2->CreateVertexVariableBinding<glm::mat4>("WorldData");
		binding2.Set(mvp);

		struct Vertex
		{
			glm::vec3 Position;
			glm::vec3 Normal;
			glm::vec2 UV0;
		};

		SharedPtr<std::string> icosphereFbxSrc = GetAssetLibrary()->Load<std::string>("Icosphere");
		Assimp::Importer importer;
		const char *data = icosphereFbxSrc->c_str();

		const aiScene *scene = importer.ReadFile("C:/Dev/C++/cgf/samples/dev/models/Icosphere.fbx", aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

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
		GetGraphicsContext()->GetRenderDevice()->CreateBuffer(vbufferDesc, &vdata, &vbuffer);

		RefCntAutoPtr<IBuffer> ibuffer;
		BufferData idata(indices.data(), indices.size() * sizeof(unsigned int));
		BufferDesc ibufferDesc;
		ibufferDesc.Name = "test2";
		ibufferDesc.Size = indices.size() * sizeof(unsigned int);
		ibufferDesc.Usage = USAGE_IMMUTABLE;
		ibufferDesc.BindFlags = BIND_INDEX_BUFFER;
		GetGraphicsContext()->GetRenderDevice()->CreateBuffer(ibufferDesc, &idata, &ibuffer);

		SharedPtr<Actor> actor = SharedPtr<Actor>::Create();
		GetCurrentScene()->AddActor(actor);

		GameBase::Start();
	}

	void Tick(double dT) override
	{
		GameBase::Tick(dT);
	}

	OnTickEvent::Connection tick;
};


int main()
{
	GameBase *game = new GameDerivative;
	game->Start();

	while (!game->GetWindow()->ShouldClose())
	{
		game->GetWindow()->Poll();

		game->Tick(0.0);
		game->GetRenderer()->Render();
	}

	delete game;
	return 0;
}