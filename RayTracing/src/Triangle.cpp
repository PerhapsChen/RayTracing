#include "Triangle.h"

Triangle::Triangle(point3 a, point3 b, point3 c, std::shared_ptr<Material> m):
	mA_Position(a), mB_Position(b), mC_Position(c), material_ptr(m)
{
	m_Normal = glm::normalize(glm::cross(mB_Position - mA_Position, mC_Position - mA_Position));

	// 构造三角形的包围盒。
	auto ix = Interval(fmin(fmin(a.x, b.x), c.x), fmax(fmax(a.x, b.x), c.x));
	auto iy = Interval(fmin(fmin(a.y, b.y), c.y), fmax(fmax(a.y, b.y), c.y));
	auto iz = Interval(fmin(fmin(a.z, b.z), c.z), fmax(fmax(a.z, b.z), c.z));
	box = AABB(ix, iy, iz);
}

bool Triangle::hit(const Ray& r, Interval ray_t, HitRecord& rec) const
{
	const float EPSILON = 0.000001f;

	auto edge1 = mB_Position - mA_Position;
	auto edge2 = mC_Position - mA_Position;
	glm::vec3 h = glm::cross(r.Direction, edge2);
	float a = glm::dot(edge1, h);

	if (a > -EPSILON && a < EPSILON) // 射线与三角形平行
		return false;
	float f = 1.0f / a;
	auto s = r.Origin - mA_Position;

	float u = f * glm::dot(s, h);
	if (u < 0.0f || u > 1.0f)
		return false;

	auto q = glm::cross(s, edge1);

	float v = f * glm::dot(r.Direction, q);
	if (v < 0.0f || u + v > 1.0f)
		return false;

	rec.t = f * dot(q, edge2);
	if (rec.t > EPSILON)
	{
		rec.material_ptr = material_ptr;
		rec.p = r.at(rec.t);
		rec.normal = m_Normal;
		rec.set_face_normal(r, m_Normal);
		rec.u = 0.5f;
		rec.v = 0.5f;
		return true;
	}
	//float uu = f * dot(s, h);
	//float vv = f * dot(q, r.Direction);
	//float ww = 1.0f - uu - vv;
	
	return false;
}

AABB Triangle::bounding_box() const
{
	return box;
}