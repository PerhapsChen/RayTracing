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
#include "Triangle.h"
#include "BVHNode.h"
#include <typeinfo>

#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "tiny_obj_loader.h"


using namespace Walnut;
#define RANDOM_FLOAT Walnut::Random::Float()

static HittableList s_FourSpheres;
static HittableList s_FourSpheres_BVH;

static HittableList s_SingleTriangle;
//static HittableList s_SingleTriangle_BVH;

static HittableList s_MultiSpheres;
static HittableList s_MultiSpheres_BVH;

static HittableList s_BunnyMesh_Diele;
static HittableList s_BunnyMeshBVH_Diele;

static HittableList s_BunnyMesh_Metal;
static HittableList s_BunnyMeshBVH_Metal;

class ExampleLayer : public Walnut::Layer
{
public:
	ExampleLayer()
		: m_Camera(45.0f, 0.1f, 100.0f) 
	{
		m_Camera.SetPosition(glm::vec3(-1.57f, 2.27f, 6.20f));
		m_Camera.SetDirection(glm::vec3(0.44f, -0.10f, -0.90f));

		// 1. 
		auto tex = std::make_shared<SolidColorTexture>(color(0.2, 0.9, 0.9));
		auto light = std::make_shared<SolidColorTexture>(color(0.9, 0.9, 0.0) * 5.0f);
		auto groundTex = std::make_shared<SolidColorTexture>(color(0.5, 0.5, 0.5));
		s_FourSpheres.add(std::make_shared<Sphere>(point3(0, -1000, 0), 1000, std::make_shared<LambertianMaterial>(groundTex)));
		s_FourSpheres.add(std::make_shared<Sphere>(point3(0, 1, 0), 1, std::make_shared<LambertianMaterial>(tex)));
		s_FourSpheres.add(std::make_shared<Sphere>(point3(0.6, 0.6, 1.9), 0.6, std::make_shared<DielectricMaterial>(1.5)));
		s_FourSpheres.add(std::make_shared<Sphere>(point3(15, 7.5, -15), 5.0, std::make_shared<DiffuseLightMaterial>(light)));
		s_FourSpheres.add(std::make_shared<Sphere>(point3(2.6, 0.6, 1.8), 0.6, std::make_shared<MetalMaterial>(glm::vec3(0.5f, 0.5f,0.5f), 0.3)));
		s_FourSpheres_BVH.add(std::make_shared<BVHNode>(s_FourSpheres));

		// 2.
		auto mat = std::make_shared<LambertianMaterial>(groundTex);
		point3 A{ 0.6, 0.6, 1.9 };
		point3 B{ 0, 1, 0 };
		point3 C{ 2.6, 0.6, 1.8 };

		s_SingleTriangle.add(std::make_shared<Triangle>(C, B, A, mat));
		//s_SingleTriangle_BVH.add(std::make_shared<BVHNode>(s_SingleTriangle));

		// 3 Multi SPheres Complex Scene.
		auto ground_material = std::make_shared<LambertianMaterial>(color(0.5, 0.5, 0.5));
		s_MultiSpheres.add(std::make_shared<Sphere>(point3(0, -1000, 0), 1000, ground_material));

		for (int a = -5; a < 5; a++) {
			for (int b = -5; b < 5; b++) {
				float choose_mat = Walnut::Random::Float();
				point3 center(a + 1.4 * Walnut::Random::Float(), 0.2, b + 1.4 * Walnut::Random::Float());

				if ((center - point3(4, 0.2, 0)).length() > 0.9) {
					std::shared_ptr<Material> sphere_material;

					if (choose_mat < 0.8) {
						// diffuse
						vec3 albedo = glm::vec3( RANDOM_FLOAT ,RANDOM_FLOAT, RANDOM_FLOAT ) * glm::vec3(RANDOM_FLOAT, RANDOM_FLOAT, RANDOM_FLOAT);
						sphere_material = std::make_shared<LambertianMaterial>(albedo);
						s_MultiSpheres.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
					}
					else if (choose_mat < 0.95) {
						// metal
						auto albedo = glm::vec3(RANDOM_FLOAT * 0.5 + 0.5, RANDOM_FLOAT * 0.5 + 0.5, RANDOM_FLOAT * 0.5 + 0.5);
						auto fuzz = RANDOM_FLOAT * 0.5 - 0.5;
						sphere_material = std::make_shared<MetalMaterial>(albedo, fuzz);
						s_MultiSpheres.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
					}
					else {
						// glass
						sphere_material = std::make_shared<DielectricMaterial>(1.5);
						s_MultiSpheres.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
					}
				}
			}
		}

		auto material1 = std::make_shared<DielectricMaterial>(1.5);
		s_MultiSpheres.add(std::make_shared<Sphere>(point3(0, 1, 0), 1.0, material1));

		auto material2 = std::make_shared<LambertianMaterial>(color(0.4, 0.2, 0.1));
		s_MultiSpheres.add(std::make_shared<Sphere>(point3(-4, 1, 0), 1.0, material2));

		auto material3 = std::make_shared<MetalMaterial>(color(0.7, 0.6, 0.5), 0.0);
		s_MultiSpheres.add(std::make_shared<Sphere>(point3(4, 1, 0), 1.0, material3));
		s_MultiSpheres_BVH.add(std::make_shared<BVHNode>(s_MultiSpheres));

		// 4. Rabbit Mesh
		std::string inputfile = "assets/models/bunny/bunny.obj";
		tinyobj::ObjReaderConfig reader_config;
		reader_config.mtl_search_path = "./"; // Path to material files

		tinyobj::ObjReader reader;
		if (!reader.ParseFromFile(inputfile, reader_config)) {
			if (!reader.Error().empty()) {
				std::cerr << "TinyObjReader: " << reader.Error();
			}
			exit(1);
		}

		if (!reader.Warning().empty()) {
			std::cout << "TinyObjReader: " << reader.Warning();
		}
		auto& attrib = reader.GetAttrib();
		auto& shapes = reader.GetShapes();

		auto mat1 = std::make_shared<MetalMaterial>(glm::vec3(0.5f, 0.5f, 0.5f), 0.3);
		auto mat2 = std::make_shared<DielectricMaterial>(2.417);

		for (size_t s = 0; s < shapes.size(); s++) 
		{
			// Loop over faces(polygon)
			size_t index_offset = 0;
			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) 
			{
				std::vector<point3> threePoints;
				// Loop over vertices in the face.
				for (size_t v = 0; v < 3; v++)
				{
					// access to vertex
					tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
					tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0] * 15.f;
					tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1] * 15.f;
					tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2] * 15.f;
					threePoints.emplace_back(point3(vx, vy, vz));
				}
				s_BunnyMesh_Metal.add(std::make_shared<Triangle>(threePoints[0], threePoints[1], threePoints[2], mat1));
				s_BunnyMesh_Diele.add(std::make_shared<Triangle>(threePoints[0], threePoints[1], threePoints[2], mat2));
				threePoints.clear();
				index_offset += 3;
			}
		}
		s_BunnyMesh_Metal.add(std::make_shared<Sphere>(point3(15, 7.5, -15), 5.0, std::make_shared<DiffuseLightMaterial>(light)));
		s_BunnyMesh_Metal.add(std::make_shared<Sphere>(point3(0, -1000, 0), 1000, std::make_shared<LambertianMaterial>(groundTex)));

		s_BunnyMesh_Diele.add(std::make_shared<Sphere>(point3(15, 7.5, -15), 5.0, std::make_shared<DiffuseLightMaterial>(light)));
		s_BunnyMesh_Diele.add(std::make_shared<Sphere>(point3(0, -1000, 0), 1000, std::make_shared<LambertianMaterial>(groundTex)));

		s_BunnyMeshBVH_Metal.add(std::make_shared<BVHNode>(s_BunnyMesh_Metal));
		s_BunnyMeshBVH_Diele.add(std::make_shared<BVHNode>(s_BunnyMesh_Diele));
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
		ImGui::Text("Camera Position: \n%s", glm::to_string(m_Camera.GetPosition()).c_str());
		ImGui::Text("Camera Direction: \n%s", glm::to_string(m_Camera.GetDirection()).c_str());
		if (ImGui::Button("Render"))
		{
			Render();
		}

		ImGui::Checkbox("Accumulate", &m_Renderer.GetSettings().Accumulate);
		if (ImGui::Button("Reset"))
			m_Renderer.ResetFrameIndex();

		ImGui::Checkbox("BVH Accelerate", &m_BVH_Accelerate);
		ImGui::DragInt("Ray Bounce", &m_Renderer.m_Bounce, 1, 1, 50);

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

		const char* items[] = { "Four Spheres","Single Triangle", "Multi Spheres", "Dielectric Bunny", "Metal Bunny", "Default"};
		ImGui::Combo("Scene", &m_SceneNum, items, IM_ARRAYSIZE(items));



		auto src_obj = m_ActivateScene->objects;
		if (src_obj.size()>0 && dynamic_cast<BVHNode*>(src_obj[0].get()))
		{
			src_obj = dynamic_cast<BVHNode*>(src_obj[0].get())->src;
		}



		for (size_t i = 0; i < src_obj.size(); i++)
		{
			if (!dynamic_cast<Sphere*>(src_obj[i].get()))
				continue;

			Sphere* sphere = dynamic_cast<Sphere*>(src_obj[i].get());

			if (m_SceneNum == 2)
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

		ImGui::End();

		ImGui::PopStyleVar();

		Render();
	}

	void Render()
	{
		Timer timer;

		m_ActivateScene = &m_Scene;
		m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);

		switch (m_SceneNum)
		{
		case(0):
			if (m_BVH_Accelerate)
				m_ActivateScene = &s_FourSpheres_BVH;
			else
				m_ActivateScene = &s_FourSpheres;
			break;
		case(1):
				m_BVH_Accelerate = true;
				m_ActivateScene = &s_SingleTriangle;
			break;
		case(2):
			if (m_BVH_Accelerate)
				m_ActivateScene = &s_MultiSpheres_BVH;
			else
				m_ActivateScene = &s_MultiSpheres;
			break;
		case(3):
			m_ActivateScene = &s_BunnyMeshBVH_Diele;
			m_BVH_Accelerate = true;
			break;
		case(4):
			m_ActivateScene = &s_BunnyMeshBVH_Metal;
			m_BVH_Accelerate = true;
			break;
		default:
			m_ActivateScene = &m_Scene;
			break;
		}

		//if (m_BVH_Accelerate)
			

		m_Renderer.Render(*m_ActivateScene, m_Camera);

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
	HittableList* m_ActivateScene = &m_Scene;
	std::vector<Hittable> ImGui_Register;
	int m_SceneNum = 0;
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