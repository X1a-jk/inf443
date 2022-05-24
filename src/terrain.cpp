
#include "terrain.hpp"


using namespace cgp;

// Evaluate 3D position of the terrain for any (u,v) \in [0,1]
float evaluate_terrain_height(float x, float y)
{
    vec2 p_i[4] = { {-10,-10}, {5,5}, {-3,4}, {6,4} };
    float h_i[4] = { 3.0f, -1.5f, 1.0f, 2.0f };
    float sigma_i[4] = { 10.0f, 3.0f, 4.0f, 4.0f };

    vec2 p_0 = { 0, 0 };
    float h_0 = 2.0f;
    float sigma_0 = 3.0f;

    float z_new = 0.0f;
    for (int i = 0; i < 4; i++) {
        float d_i = norm(vec2(x, y) - 0.9*p_i[i]) / (0.9*sigma_i[i]);
        float z_i = 2.0f*h_i[i] * std::exp(-d_i * d_i);
        z_new += z_i;
    }

    float d = norm(vec2(x, y) - p_0) / sigma_0;

    float z = h_0 * std::exp(-d * d);

    return z_new;
}

float evaluate_pond_height(float x, float y, float terrain_length)
{
    
    if (x * x/9.0f + y * y/16.0f <= terrain_length * terrain_length / 400.0f) {
        return evaluate_terrain_height(x, y);
    }
    else {
        return evaluate_terrain_height(x, y)-0.1f;
    }
    
    
    
}

mesh create_terrain_mesh(int N, float terrain_length)
{

    mesh terrain; // temporary terrain storage (CPU only)
    terrain.position.resize(N*N);
    terrain.uv.resize(N * N);

    // Fill terrain geometry
    for(int ku=0; ku<N; ++ku)
    {
        for(int kv=0; kv<N; ++kv)
        {
            // Compute local parametric coordinates (u,v) \in [0,1]
            float u = ku/(N-1.0f);
            float v = kv/(N-1.0f);

            // Compute the real coordinates (x,y) of the terrain 
            float x = (u - 0.5f) * terrain_length;
            float y = (v - 0.5f) * terrain_length;

            // Compute the surface height function at the given sampled coordinate
            float z = evaluate_terrain_height(x,y);

            // Store vertex coordinates
            terrain.position[kv+N*ku] = {x,y,z};
            terrain.uv[kv + N * ku] = {u,v};
        }
    }

    // Generate triangle organization
    //  Parametric surface with uniform grid sampling: generate 2 triangles for each grid cell
    for(int ku=0; ku<N-1; ++ku)
    {
        for(int kv=0; kv<N-1; ++kv)
        {
            unsigned int idx = kv + N*ku; // current vertex offset

            uint3 triangle_1 = {idx, idx+1+N, idx+1};
            uint3 triangle_2 = {idx, idx+N, idx+1+N};

            terrain.connectivity.push_back(triangle_1);
            terrain.connectivity.push_back(triangle_2);
        }
    }

    // need to call this function to fill the other buffer with default values (normal, color, etc)
	terrain.fill_empty_field(); 

    return terrain;
}

mesh create_pond_mesh(int N, float terrain_length)
{

    mesh terrain; // temporary terrain storage (CPU only)
    terrain.position.resize(N * N);
    terrain.uv.resize(N * N);

    // Fill terrain geometry
    for (int ku = 0; ku < N; ++ku)
    {
        for (int kv = 0; kv < N; ++kv)
        {
            // Compute local parametric coordinates (u,v) \in [0,1]
            float u = ku / (N - 1.0f);
            float v = kv / (N - 1.0f);
            
            // Compute the real coordinates (x,y) of the terrain 
            float x = (u - 0.5f) * terrain_length;
            float y = (v - 0.5f) * terrain_length;

            // Compute the surface height function at the given sampled coordinate
            float z = evaluate_pond_height(x, y,terrain_length);

            // Store vertex coordinates
            
            terrain.position[kv + N * ku] = { x,y,z };
            terrain.uv[kv + N * ku] = { u,v };
        }
    }

    // Generate triangle organization
    //  Parametric surface with uniform grid sampling: generate 2 triangles for each grid cell
    for (int ku = 0; ku < N - 1; ++ku)
    {
        for (int kv = 0; kv < N - 1; ++kv)
        {
            unsigned int idx = kv + N * ku; // current vertex offset

            uint3 triangle_1 = { idx, idx + 1 + N, idx + 1 };
            uint3 triangle_2 = { idx, idx + N, idx + 1 + N };

            terrain.connectivity.push_back(triangle_1);
            terrain.connectivity.push_back(triangle_2);
        }
    }

    // need to call this function to fill the other buffer with default values (normal, color, etc)
    terrain.fill_empty_field();

    return terrain;
}

std::vector<cgp::vec3> generate_positions_on_terrain(int N, float terrain_length) {
    std::vector<cgp::vec3> pos;
    for (int i = 0; i < N; i++) {
        int ku = rand_interval(1, N);
        int kv = rand_interval(1, N);
        float u = ku / (N - 1.0f);
        float v = kv / (N - 1.0f);

        // Compute the real coordinates (x,y) of the terrain 
        float x = (u - 0.5f) * terrain_length;
        float y = (v - 0.5f) * terrain_length;

        // Compute the surface height function at the given sampled coordinate
        float z = evaluate_terrain_height(x, y);

        pos.push_back(vec3(x, y, z));
    }
    return pos;
}

std::vector<cgp::vec3> generate_positions_trees(int N, float terrain_length) {
    std::vector<cgp::vec3> pos;
    float epsilon = 0.001f;
    float L = terrain_length;
    float gap = L / N;
    float y1 = 2.5f / 6.0f * L;
    float y2 = 1.5f / 6.0f * L;
    float y3 = -2.4f / 6.0f * L;
    float y4 = -1.4f / 6.0f * L;

    for (float x = (-0.25f) * terrain_length; x < 3.0f / 8.0f * L; x += gap) {
        float ku = rand_interval(0, 3)-1.0f;
        float kv = rand_interval(0, 3) - 1.0f;
        //float x1 = x + ku / 100.0f*L;
        //float y1 = y1 + kv / 100.0f*L;
        float z1= evaluate_terrain_height(x, y1);
        pos.push_back(vec3(x, y1, z1));

        ku = rand_interval(0, 3) - 1;
        kv = rand_interval(0, 3) - 1;
        //float x2 = x + ku / 20.0f;
        //float y2 = y2 + kv / 20.0f;
        float z2 = evaluate_terrain_height(x, y2);
        pos.push_back(vec3(x, y2, z2));

        ku = rand_interval(0, 3) - 1;
        kv = rand_interval(0, 3) - 1;
        //float x3 = x + ku / 10.0f;
       // float y3 = y3 + kv / 10.0f ;
        float z3 = evaluate_terrain_height(x, y3);
        pos.push_back(vec3(x, y3, z3));

        ku = rand_interval(0, 3) - 1;
        kv = rand_interval(0, 3) - 1;
        //float x4 = x + ku / 10.0f ;
        //float y4 = y4 + kv / 10.0f ;
        float z4 = evaluate_terrain_height(x, y4);
        pos.push_back(vec3(x, y4, z4));
    }

    /*
    for (int ku = 0; ku < N; ++ku) {
        for (int kv = 0; kv < N; ++kv) {
            float u = ku / (N - 1.0f);
            float v = kv / (N - 1.0f);
            float x = (u - 0.5f) * terrain_length;
            float y = (v - 0.5f) * terrain_length;

            if (((x ) * (x) / (4.0f * L * L / 125.0f) + y * y / (16.0f * L * L / 125.0f) - 1.0f <= epsilon) || ((x ) * (x ) / (L * L / 116.0f) + y * y / (4.0f * L * L / 125.0f) - 1.0f <= epsilon)) {
                float z = evaluate_terrain_height(x, y);

                pos.push_back(vec3(x, y, z));
            }

        }
    }
    */
    
    return pos;
}