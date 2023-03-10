#include <iostream>
#include <thread>
#include <vector>
#include <map>
#include "rtweekend.h"
#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"

hittable_list random_scene() {
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
}

color ray_color(const ray& r, const hittable& world, int depth){
    hit_record rec;

    // too much bounce, no energy to eye.
    if(depth <= 0){
        return color(0,0,0);
    }

    if(world.hit(r,0.001,infinity,rec)){ //! recur
        ray scattered;
        color attenuation;
        if(rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth-1);
        return color(0,0,0);
    }
    // else -> still background.
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t) * color(1.0,1.0,1.0) + t*color(0.5,0.7,1.0);
}


void write_many_rows(int tIdx, 
                     int NofT, 
                     int image_width, 
                     int image_height, 
                     std::vector<int>& result, 
                     hittable_list& world,
                     int max_depth, 
                     int samples_per_pixel,
                     camera& cam)
{
    int inc = image_height / NofT;   
    int r_begin = image_height - 1- inc*tIdx; 
    int r_end = image_height - inc*(tIdx+1);
    for(int j = r_begin; j >= r_end; --j){
        // std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for(int i = 0; i < image_width; ++i){
            color pixel_color(0,0,0);
            for(int s = 0; s < samples_per_pixel; ++s){
                auto u = (i+random_double())/(image_width-1);
                auto v = (j+random_double())/(image_height-1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r,world, max_depth);
            }    
            write_color_mt(pixel_color , samples_per_pixel, result);        
        }
    }
    std::cerr << "Thread " << tIdx << " done.\n";
}


int main() {

    const auto aspect_ratio = 3.0 / 2.0;
    const int image_width = 1200;
    const int image_height = static_cast<int> (image_width/aspect_ratio);
    const int samples_per_pixel = 500;
    const int max_depth = 50;

    //World

    auto world = random_scene();

    //Camera

    point3 lookfrom(13,2,3);
    point3 lookat(0,0,0);
    vec3 vup(0,1,0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.1;

    camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

    std::map<int, std::vector<int>> result {}; //- store multi thread result 
    std::vector<std::thread> thread_vec {};
    int NofT = 80;

    for(int t = 0; t < NofT; t++)
    {
        result[t].reserve(static_cast<int>(image_width*image_width/aspect_ratio*3));
    }
    for(int t = 0; t < NofT; t++)
    {
        thread_vec.push_back(std::thread(write_many_rows,
            t,NofT,image_width,image_height,std::ref(result[t]),std::ref(world),max_depth,samples_per_pixel,std::ref(cam)));
    }
    for(int t = 0; t < NofT; t++){
        thread_vec[t].join();
    }

    const std::string outputFile {"/home/chen-p/perhaps/RayTracing/output/Class1Final.ppm"};
    std::ofstream fout(outputFile);
    fout << "P3\n" << image_width << " " << image_height <<"\n255\n";
    
    for(int t = 0; t < NofT; t++){
        auto& vec = result[t];
        for(auto it = vec.begin(); it!=vec.end(); it++){
            fout << *it << " ";
        }
    }

    fout.close();

    std::cerr << "\nDone.\n";
}