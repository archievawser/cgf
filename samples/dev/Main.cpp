#include "cgf.h"

#include <memory>

#include "core/Memory.h"


class GameDerivative : public GameBase
{
public:
	void Start() override
	{
		{
			struct Test : ManagedObject
			{
				int e = 5;
			};

			Test* raw = new Test;

			{
				SharedPtr<Test> test(raw);

				std::cout << test->e << std::endl;

				SharedPtr<Test> abc = test;

				std::cout << abc->e << std::endl;
			}
		}

		auto basematerial = GetAssetLibrary()->Load<Material>("Red");
		auto material1 = std::make_shared<MaterialInstance>(basematerial);

		basematerial = GetAssetLibrary()->Load<Material>("Green");
		auto material2 = std::make_shared<MaterialInstance>(basematerial);

		glm::mat4 proj = glm::perspectiveFov(70.f, 1920.f, 1080.f, 0.1f, 1000.f);
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -10));
		glm::mat4 model = glm::mat4x4(1.0f);

		glm::mat4x4 mvp1 = proj * view * glm::translate(model, glm::vec3(1, 0, 0));
		glm::mat4x4 mvp2 = proj * view * glm::translate(model, glm::vec3(-1, 0, 0));

		DeviceVarBinding<glm::mat4> binding1 = material1->CreateVertexVariableBinding<glm::mat4>("WorldData");
		binding1.Set(mvp1);

		DeviceVarBinding<glm::mat4> binding2 = material2->CreateVertexVariableBinding<glm::mat4>("WorldData");
		binding2.Set(mvp2);

		float vertexData[] { 0.5f, 0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f };
		unsigned int indexData[] { 0, 1, 2, 2, 3, 0};

		RefCntAutoPtr<IBuffer> vbuffer;
		BufferData vdata(vertexData, sizeof(vertexData));
		BufferDesc vbufferDesc;
		vbufferDesc.Name = "test";
		vbufferDesc.Size = sizeof(vertexData);
		vbufferDesc.Usage = USAGE_IMMUTABLE;
		vbufferDesc.BindFlags = BIND_VERTEX_BUFFER;
		GetRenderer()->GetRenderDevice()->CreateBuffer(vbufferDesc, &vdata, &vbuffer);

		RefCntAutoPtr<IBuffer> ibuffer;
		BufferData idata(indexData, sizeof(indexData));
		BufferDesc ibufferDesc;
		ibufferDesc.Name = "test2";
		ibufferDesc.Size = sizeof(indexData);
		ibufferDesc.Usage = USAGE_IMMUTABLE;
		ibufferDesc.BindFlags = BIND_INDEX_BUFFER;
		GetRenderer()->GetRenderDevice()->CreateBuffer(ibufferDesc, &idata, &ibuffer);
  
		m_e2ee = GetRenderer()->DrawChain.CreateCommand(ibuffer, vbuffer, material2, 6);
		m_eee = GetRenderer()->DrawChain.CreateCommand(ibuffer, vbuffer, material1, 6);
	}

	std::shared_ptr<EntityDrawCmdHandle> m_e2ee;
	std::shared_ptr<EntityDrawCmdHandle> m_eee;
};

IMPLEMENT_GAME_ENTRY_POINT( GameDerivative )