#include "HittableList.h"

bool HittableList::hit(const Ray& ray, float t_min, float t_max, HitRecord& record) const
{
	HitRecord tmp_record;
	bool hit_anything = false;
	float closest_so_far = t_max;

	for (const auto& object : objects) 
	{
		if (object->hit(ray, t_min, closest_so_far, tmp_record))
		{
			hit_anything = true;
			closest_so_far = tmp_record.t;
			record = tmp_record;
		}
	}
	return hit_anything;
}

bool HittableList::bounding_box(float time0, float time1, AABB& output_box) const
{
	//AABB temp_box;
	//bool first_box = true;
	//for (const auto& object : objects)
	//{
	//	if (!object->bounding_box(time0, time1, temp_box))
	//		return false;
	//	output_box = first_box ? temp_box : AABB::surrounding_box(output_box, temp_box); //! confused.
	//	first_box = false;
	//}
	return true;
}