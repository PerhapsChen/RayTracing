#include "Renderer.h"

#include "Walnut/Random.h"
#include <execution>
#include "Utils.h"

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (m_FinalImage)
	{
		// No resize necessary
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
			return;

		m_FinalImage->Resize(width, height);
	}
	else
	{
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];

	delete[] m_AccumulationData;
	m_AccumulationData = new glm::vec4[width * height];

	m_ImageHorizontalIter.resize(width);
	m_ImageVerticalIter.resize(height);
	for (uint32_t i = 0; i < width; i++) {
		m_ImageHorizontalIter[i] = i;
	}
	for (uint32_t i = 0; i < height; i++) {
		m_ImageVerticalIter[i] = i;
	}

}

void Renderer::Render(const HittableList& scene, const Camera& camera)
{
	m_ActiveScene = &scene;
	m_ActiveCamera = &camera;

	if (m_FrameIndex == 1)
		memset(m_AccumulationData, 0, m_FinalImage->GetWidth() * m_FinalImage->GetHeight() * sizeof(glm::vec4));

	std::for_each(std::execution::par, m_ImageVerticalIter.begin(), m_ImageVerticalIter.end(),
		[this](uint32_t y)
		{
			std::for_each(std::execution::par, m_ImageHorizontalIter.begin(), m_ImageHorizontalIter.end(),
			[this, y](uint32_t x)
				{
					glm::vec4 color = PerPixel(x, y);
					m_AccumulationData[x + y * m_FinalImage->GetWidth()] += color;

					glm::vec4 accumulatedColor = m_AccumulationData[x + y * m_FinalImage->GetWidth()];
					accumulatedColor /= (float)m_FrameIndex;

					accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));
					m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(accumulatedColor);
				});
		}); 

	m_FinalImage->SetData(m_ImageData);

	if (m_Settings.Accumulate)
		m_FrameIndex++;
	else
		m_FrameIndex = 1;
}

glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y)
{
	// 获取从相机到像素的光线 (eye ray)
	Ray ray;
	ray.Origin = m_ActiveCamera->GetPosition();
	ray.Direction = m_ActiveCamera->GetRayDirections()[x + y * m_FinalImage->GetWidth()];

	glm::vec3 pixel_color(0.0f, 0.0f, 0.0f);
	glm::vec3 background(0.7f, 0.8f, 1.0f);
	int bounce = 30;
	pixel_color = RayColor(ray, background, m_Bounce);
	return glm::vec4(pixel_color, 1.0f);
}

glm::vec3 Renderer::RayColor(const Ray& ray, const color& background, int depth)
{
	HitRecord record;
	if (depth <= 0)
		return color(0.0f, 0.0f, 0.0f);

	if (!m_ActiveScene->hit(ray, Interval(0.001f, Utils::INF), record))
		return background;

	Ray scattered;
	color attenuation;
	color emitted = record.material_ptr->emitted(record.u, record.v, record.p);

	if (!record.material_ptr->scatter(ray, record, attenuation, scattered))
		return emitted;

	return emitted + attenuation * RayColor(scattered, background, depth - 1);
}