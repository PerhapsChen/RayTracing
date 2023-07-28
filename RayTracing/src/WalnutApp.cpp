#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include "Renderer.h"
#include "Camera.h"
#include "LambertianMaterial.h"
#include "DiffuseLightMaterial.h"
#include "DielectricMaterial.h"
#include "MetalMaterial.h"
#include "Texture.h"
#include "Sphere.h"
#include "BVHNode.h"
#include <typeinfo>


#include <glm/gtc/type_ptr.hpp>

using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:
	ExampleLayer()
		: m_Camera(45.0f, 0.1f, 100.0f) 
	{

		//auto tex = std::make_shared<SolidColorTexture>(color(0.2, 0.9, 0.9));
		//auto light = std::make_shared<SolidColorTexture>(color(0.9, 0.9, 0.0) * 5.0f);
		//auto groundTex = std::make_shared<SolidColorTexture>(color(0.5, 0.5, 0.5));
		//m_Scene.add(std::make_shared<Sphere>(point3(0, -1000, 0), 1000, std::make_shared<LambertianMaterial>(groundTex)));
		//m_Scene.add(std::make_shared<Sphere>(point3(0, 1, 0), 1, std::make_shared<LambertianMaterial>(tex)));
		//m_Scene.add(std::make_shared<Sphere>(point3(0.6, 0.6, 1.9), 0.6, std::make_shared<DielectricMaterial>(1.5)));
		//m_Scene.add(std::make_shared<Sphere>(point3(15, 7.5, -15), 5.0, std::make_shared<DiffuseLightMaterial>(light)));
		//m_Scene.add(std::make_shared<Sphere>(point3(2.6, 0.6, 1.8), 0.6, std::make_shared<MetalMaterial>(glm::vec3(0.5f, 0.5f,0.5f), 0.3)));

		//m_BVHScene.add(std::make_shared<BVHNode>(m_Scene, 0, 10000));


		//m_Camera.SetPosition(vec3(-13, 2, 3));
		//m_Camera.SetDirection(vec3(0, 0, 0));


#define RANDOM_FLOAT Walnut::Random::Float()

		auto ground_material = std::make_shared<LambertianMaterial>(color(0.5, 0.5, 0.5));
		m_Scene.add(std::make_shared<Sphere>(point3(0, -1000, 0), 1000, ground_material));

		for (int a = -11; a < 11; a++) {
			for (int b = -11; b < 11; b++) {
				float choose_mat = Walnut::Random::Float();
				point3 center(a + 0.9 * Walnut::Random::Float(), 0.2, b + 0.9 * Walnut::Random::Float());

				if ((center - point3(4, 0.2, 0)).length() > 0.9) {
					std::shared_ptr<Material> sphere_material;

					if (choose_mat < 0.8) {
						// diffuse
						vec3 albedo = glm::vec3( RANDOM_FLOAT ,RANDOM_FLOAT, RANDOM_FLOAT ) * glm::vec3(RANDOM_FLOAT, RANDOM_FLOAT, RANDOM_FLOAT);
						sphere_material = std::make_shared<LambertianMaterial>(albedo);
						m_Scene.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
					}
					else if (choose_mat < 0.95) {
						// metal
						auto albedo = glm::vec3(RANDOM_FLOAT * 0.5 + 0.5, RANDOM_FLOAT * 0.5 + 0.5, RANDOM_FLOAT * 0.5 + 0.5);
						auto fuzz = RANDOM_FLOAT * 0.5 - 0.5;
						sphere_material = std::make_shared<MetalMaterial>(albedo, fuzz);
						m_Scene.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
					}
					else {
						// glass
						sphere_material = std::make_shared<DielectricMaterial>(1.5);
						m_Scene.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
					}
				}
			}
		}

		auto material1 = std::make_shared<DielectricMaterial>(1.5);
		m_Scene.add(std::make_shared<Sphere>(point3(0, 1, 0), 1.0, material1));

		auto material2 = std::make_shared<LambertianMaterial>(color(0.4, 0.2, 0.1));
		m_Scene.add(std::make_shared<Sphere>(point3(-4, 1, 0), 1.0, material2));

		auto material3 = std::make_shared<MetalMaterial>(color(0.7, 0.6, 0.5), 0.0);
		m_Scene.add(std::make_shared<Sphere>(point3(4, 1, 0), 1.0, material3));


		m_BVHScene.clear();
		m_BVHScene.add(std::make_shared<BVHNode>(m_Scene));

	}

	virtual void OnUpdate(float ts) override
	{
		if (m_Camera.OnUpdate(ts))
			m_Renderer.ResetFrameIndex();
	}

	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Text("Last render: %.3fms", m_LastRenderTime);
		ImGui::Text("FPS: %.1f", 1000/m_LastRenderTime);
		if (ImGui::Button("Render"))
		{
			Render();
		}

		ImGui::Checkbox("Accumulate", &m_Renderer.GetSettings().Accumulate);
		if (ImGui::Button("Reset"))
			m_Renderer.ResetFrameIndex();

		ImGui::Checkbox("BVH Accelerate", &m_BVH_Accelerate);

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

		m_ViewportWidth = ImGui::GetContentRegionAvail().x;
		m_ViewportHeight = ImGui::GetContentRegionAvail().y;

		auto image = m_Renderer.GetFinalImage();
		if (image)
			ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() },
				ImVec2(0, 1), ImVec2(1, 0));

		ImGui::End();

		ImGui::Begin("Scene");
		for (size_t i = 0; i < m_Scene.objects.size(); i++)
		{
			
			Sphere* sphere = dynamic_cast<Sphere*>(m_Scene.objects[i].get());
			
			if (sphere->radius <= 0.5)
				continue;

			ImGui::PushID(i);
			ImGui::DragFloat3("Position", glm::value_ptr(sphere->center), 0.1f);
			ImGui::DragFloat("Radius", &sphere->radius, 0.1f);

			if (dynamic_cast<DiffuseLightMaterial*>(sphere->material_ptr.get()))
			{
				auto mat = dynamic_cast<DiffuseLightMaterial*>(sphere->material_ptr.get());
				auto tex = dynamic_cast<SolidColorTexture*>(mat->emit.get());
				ImGui::ColorEdit3("Light Color", glm::value_ptr(tex->color_value));

			}

			if (dynamic_cast<LambertianMaterial*>(sphere->material_ptr.get()))
			{
				auto mat = dynamic_cast<LambertianMaterial*>(sphere->material_ptr.get());
				auto tex = dynamic_cast<SolidColorTexture*>(mat->albedo.get());
				ImGui::ColorEdit3("Diffuse Color", glm::value_ptr(tex->color_value));
			}

			if (dynamic_cast<DielectricMaterial*>(sphere->material_ptr.get()))
			{
				auto mat = dynamic_cast<DielectricMaterial*>(sphere->material_ptr.get());
				ImGui::DragFloat("Refract Index", &mat->ir, 0.1f, -1.0f, 3.0f);
			}

			if (dynamic_cast<MetalMaterial*>(sphere->material_ptr.get()))
			{
				auto mat = dynamic_cast<MetalMaterial*>(sphere->material_ptr.get());
				ImGui::DragFloat("Refract Index", &mat->fuzz, 0.05f, 0.0f, 1.0f);
				ImGui::ColorEdit3("Diffuse Color", glm::value_ptr(mat->albedo));
			}

			ImGui::Separator();

			ImGui::PopID();
		}

		//for (size_t i = 0; i < m_Scene.Materials.size(); i++)
		//{
		//	ImGui::PushID(i);

		//	Material& material = m_Scene.Materials[i];
		//	ImGui::ColorEdit3("Albedo", glm::value_ptr(material.Albedo));
		//	ImGui::DragFloat("Roughness", &material.Roughness, 0.05f, 0.0f, 1.0f);
		//	ImGui::DragFloat("Metallic", &material.Metallic, 0.05f, 0.0f, 1.0f);

		//	ImGui::Separator();

		//	ImGui::PopID();
		//}
		ImGui::End();

		ImGui::PopStyleVar();

		Render();
	}

	void Render()
	{
		Timer timer;

		m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);
		if(m_BVH_Accelerate)
			m_Renderer.Render(m_BVHScene, m_Camera);
		else
			m_Renderer.Render(m_Scene, m_Camera);

		m_LastRenderTime = timer.ElapsedMillis();
	}
private:
	Renderer m_Renderer;
	Camera m_Camera;
	HittableList m_Scene;
	HittableList m_BVHScene;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
	bool m_BVH_Accelerate = false;
	float m_LastRenderTime = 0.0f;
	std::vector<Hittable> ImGui_Register;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Tracing";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit"))
				{
					app->Close();
				}
				ImGui::EndMenu();
			}
		});
	return app;
}