#include "tree.hpp"

using namespace cgp;


mesh create_cylinder_mesh(float radius, float height)
{
    mesh m;

    // Number of samples
    int N = 20;
    //m.position.resize(N * N);
    //m.uv.resize(N * N);

    // Geometry
    
    for (int k = 0; k < N; ++k)
    {
        float u = k / float(N);
        vec3 p = { radius * std::cos(2 * 3.14f * u), radius * std::sin(2 * 3.14f * u), 0.0f };
        vec2 uv = { std::cos(2 * 3.14f * u),std::cos(2 * 3.14f * u) };
        m.position.push_back(p);
        m.position.push_back(p + vec3(0, 0, height));
        m.uv.push_back(uv);
    }
    
    /*

    for (int ku = 0; ku < N; ++ku)
    {
        for (int kv = 0; kv < N; ++kv)
        {
            // Compute local parametric coordinates (u,v) \in [0,1]
            float u = ku / (N - 1.0f);
            float v = kv / (N - 1.0f);

            // Compute the local surface function
            vec3 p = { radius * std::cos(2 * 3.14f * u), radius * std::sin(2 * 3.14f * u), 0.0f };
            vec2 uv = { std::cos(2 * 3.14f * u),std::cos(2 * 3.14f * u) };

            // Store vertex coordinates
            m.position[kv + N * ku] = p;
            m.uv[kv + N * ku] = uv;
        }
    }
    */
    // Connectivity
    for (int k = 0; k < N; ++k)
    {
        int u00 = 2 * k;
        int u01 = (2 * k + 1) % (2 * N);
        int u10 = (2 * (k + 1)) % (2 * N);
        int u11 = (2 * (k + 1) + 1) % (2 * N);

        uint3 t1 = { u00, u10, u11 };
        uint3 t2 = { u00, u11, u01 };
        m.connectivity.push_back(t1);
        m.connectivity.push_back(t2);
    }

    m.fill_empty_field();
    return m;
}

mesh create_cylinder_down_mesh(float radius, float height)
{
    mesh m;

    // Number of samples
    int N = 20;

    // Geometry
    for (int k = 0; k < N; ++k)
    {
        float u = k / float(N);
        vec3 p = { 0.0f, radius * std::cos(2 * 3.14f * u) , radius * std::sin(2 * 3.14f * u) };
        m.position.push_back(p);
        m.position.push_back(p + vec3(height,0,0));
    }

    // Connectivity
    for (int k = 0; k < N; ++k)
    {
        int u00 = 2 * k;
        int u01 = (2 * k + 1) % (2 * N);
        int u10 = (2 * (k + 1)) % (2 * N);
        int u11 = (2 * (k + 1) + 1) % (2 * N);

        uint3 t1 = { u00, u10, u11 };
        uint3 t2 = { u00, u11, u01 };
        m.connectivity.push_back(t1);
        m.connectivity.push_back(t2);
    }

    m.fill_empty_field();
    return m;
}

mesh create_cone_mesh(float radius, float height, float z_offset)
{
    mesh m;

    // conical structure
    // *************************** //

    int N = 20;

    // geometry
    for (int k = 0; k < N; ++k)
    {
        float u = k / float(N);
        vec3 p = { radius * std::cos(2 * 3.14f * u), radius * std::sin(2 * 3.14f * u), 0.0f };
        m.position.push_back(p + vec3{ 0,0,z_offset });
    }
    // apex
    m.position.push_back({ 0,0,height + z_offset });

    // connectivity
    int Ns = N;
    for (int k = 0; k < Ns; ++k) {
        m.connectivity.push_back({ k , (k + 1) % Ns, Ns });
    }

    // close the bottom of the cone
    // *************************** //

    // Geometry
    for (int k = 0; k < N; ++k)
    {
        float u = k / float(N);
        vec3 p = { radius * std::cos(2 * 3.14f * u), radius * std::sin(2 * 3.14f * u), 0.0f };
        m.position.push_back(p + vec3{ 0,0,z_offset });
    }
    // central position
    m.position.push_back({ 0,0,z_offset });

    // connectivity
    for (int k = 0; k < Ns; ++k)
        m.connectivity.push_back({ k + Ns + 1, (k + 1) % Ns + Ns + 1, 2 * Ns + 1 });

    m.fill_empty_field();
    return m;
}

mesh create_tree()
{
    float h = 0.7f; // trunk height
    float r = 0.1f; // trunk radius

    // Create a brown trunk
    mesh trunk = create_cylinder_mesh(r, h);
    trunk.color.fill({ 0.4f, 0.3f, 0.3f });


    // Create a green foliage from 3 cones
    mesh foliage = create_cone_mesh(4 * r, 6 * r, 0.0f);      // base-cone
    foliage.push_back(create_cone_mesh(4 * r, 6 * r, 2 * r));   // middle-cone
    foliage.push_back(create_cone_mesh(4 * r, 6 * r, 4 * r));   // top-cone
    foliage.position += vec3(0, 0, h);                 // place foliage at the top of the trunk
    foliage.color.fill({ 0.4f, 0.6f, 0.3f });

    // The tree is composed of the trunk and the foliage
    mesh tree = trunk;
    tree.push_back(foliage);

    return tree;
}


mesh create_parapet(float height)
{
    float h = height; // trunk height
    float r = 0.1f; // trunk radius

    

    // Create a brown trunk
    mesh trunk_left = create_cylinder_mesh(r, h);
    trunk_left.color.fill({ 1.0f, 1.0f, 1.0f });


    mesh trunk_right = create_cylinder_mesh(r, h);
    trunk_right.position += vec3{ h*3,0.0f,0.0f };
    trunk_right.color.fill({ 1.0f, 1.0f, 1.0f });

    mesh trunk_up = create_cylinder_down_mesh(r/2, h*3);    
    trunk_up.position += vec3{ 0.0f,0.0f,h-r/2 };
    trunk_up.color.fill({ 1.0f, 1.0f, 0.0f });

    mesh trunk_mid = create_cylinder_down_mesh(r / 2, h * 3);
    trunk_mid.position += vec3{ 0.0f,0.0f,h*2/3 - r / 2 };
    trunk_mid.color.fill({ 0.0f, 1.0f, 0.0f });

    mesh trunk_down = create_cylinder_down_mesh(r / 2, h * 3);
    trunk_down.position += vec3{ 0.0f,0.0f,h/3 - r / 2 };
    trunk_down.color.fill({ 1.0f, 1.0f, 0.0f });


    // Create a green foliage from 3 cones


    // The tree is composed of the trunk and the foliage
    mesh tree = trunk_left;
    tree.push_back(trunk_right);
    tree.push_back(trunk_up);
    tree.push_back(trunk_mid);
    tree.push_back(trunk_down);

    return tree;
}



