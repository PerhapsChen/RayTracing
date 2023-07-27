#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include "Renderer.h"
#include "Camera.h"
#include "Material.h"
#include "LambertianMaterial.h"
#include "DiffuseLightMaterial.h"
#include "DielectricMaterial.h"
#include "MetalMaterial.h"
#include "Texture.h"
#include "Sphere.h"
#include <typeinfo>


#include <glm/gtc/type_ptr.hpp>

using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:
	ExampleLayer()
		: m_Camera(45.0f, 0.1f, 100.0f) 
	{
		auto tex = std::make_shared<SolidColorTexture>(color(0.2, 0.9, 0.9));
		auto light = std::make_shared<SolidColorTexture>(color(0.9, 0.9, 0.0) * 5.0f);
		auto groundTex = std::make_shared<SolidColorTexture>(color(0.5, 0.5, 0.5));
		m_Scene.add(std::make_shared<Sphere>(point3(0, -1000, 0), 1000, std::make_shared<LambertianMaterial>(groundTex)));
		m_Scene.add(std::make_shared<Sphere>(point3(0, 1, 0), 1, std::make_shared<LambertianMaterial>(tex)));
		m_Scene.add(std::make_shared<Sphere>(point3(0.6, 0.6, 1.9), 0.6, std::make_shared<DielectricMaterial>(1.5)));
		m_Scene.add(std::make_shared<Sphere>(point3(15, 7.5, -15), 5.0, std::make_shared<DiffuseLightMaterial>(light)));
		m_Scene.add(std::make_shared<Sphere>(point3(2.6, 0.6, 1.8), 0.6, std::make_shared<MetalMaterial>(glm::vec3(0.5f, 0.5f,0.5f), 0.3)));
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
			ImGui::PushID(i);
			Sphere* sphere = dynamic_cast<Sphere*>(m_Scene.objects[i].get());
			
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


				//ImGui::ColorEdit3("Albedo", &sphere->material_ptr, 0.1f);
			//ImGui::DragInt("Material", &sphere.MaterialIndex, 1.0f, 0, (int)m_Scene.Materials.size() - 1);

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
		m_Renderer.Render(m_Scene, m_Camera);

		m_LastRenderTime = timer.ElapsedMillis();
	}
private:
	Renderer m_Renderer;
	Camera m_Camera;
	HittableList m_Scene;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

	float m_LastRenderTime = 0.0f;
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