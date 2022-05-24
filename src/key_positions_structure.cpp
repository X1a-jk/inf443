#include "key_positions_structure.hpp"

using namespace cgp;

void keyframe_structure::initialize(buffer<vec3> const& key_positions_arg, buffer<float> const& key_times_arg)
{
	key_positions = key_positions_arg;
	key_times = key_times_arg;

	// Initialise the mesh_drawable
	mesh body_mesh = mesh_load_file_obj("model/body.obj");
	mesh_drawable body;
	body.initialize(body_mesh, "body");


	mesh front_left_mesh = mesh_load_file_obj("model/front-left2.obj");
	mesh_drawable front_left;
	front_left.initialize(front_left_mesh, "front_left");

	mesh front_right_mesh = mesh_load_file_obj("model/front-right2.obj");
	mesh_drawable front_right;
	front_right.initialize(front_right_mesh, "front_right");

	mesh back_left_mesh = mesh_load_file_obj("model/back-left2.obj");
	mesh_drawable back_left;
	back_left.initialize(back_left_mesh, "back_left");

	mesh back_right_mesh = mesh_load_file_obj("model/back-right2.obj");
	mesh_drawable back_right;
	back_right.initialize(back_right_mesh, "back_right");

	// Generat the hierarchy
	hierarchy.add(body);
	hierarchy.add(front_left, "body", { -2.5,6,2.5 });
	hierarchy.add(front_right, "body", { -2.5,6,-2.5 });
	hierarchy.add(back_left, "body", { 9.5,6,2.5 });
	hierarchy.add(back_right, "body", { 9.5,6,-2.5 });

	hierarchy["body"].transform.scaling = 0.01f;
	//sphere_key_positions.initialize(mesh_primitive_sphere(0.05f), "Sphere keyframe");
	//sphere_current.initialize(mesh_primitive_sphere(0.06f), "Sphere current");
	 
	//sphere_key_positions.initialize(tree_mesh, "Sphere keyframe");
	//sphere_current.initialize(tree_mesh, "Sphere current");
	//sphere_current.transform.rotation = rotation_transform::from_axis_angle({ 1,0,0 }, 3.14f / 2);
	
	//sphere_key_positions.shading.color = { 1,1,1 };
	//sphere_current.shading.color = { 0,0,1 };

	polygon_key_positions.initialize(key_positions);
	polygon_key_positions.color = { 1,0,0 };
}

void keyframe_structure::display_key_positions(environmentType const& environment)
{
	assert_cgp(key_times.size() == key_positions.size(), "key_time and key_positions should have the same size");

	// Display the key positions
	/*
	if (display_keyposition)
	{
		int N = key_positions.size();
		for (int k = 0; k < N; ++k) {
			if (picking.active && picking.index == k)
				sphere_key_positions.shading.color = { 1,0,0 };
			else
				sphere_key_positions.shading.color = { 1,1,1 };
			sphere_key_positions.transform.translation = key_positions[k];
			draw(sphere_key_positions, environment);
		}
	}
	*/
		

	// Display the polygon linking the key positions
	if (picking.active)
		polygon_key_positions.update(key_positions); // update the polygon if needed
	if (display_polygon)
		draw(polygon_key_positions, environment);

}

void keyframe_structure::display_current_position(vec3 const& p, environmentType& environment)
{
	// Display the interpolated position
	// sphere_current.transform.translation = p;
	hierarchy["body"].transform.translation = p;
	// draw(sphere_current, environment);
	draw(hierarchy, environment);

	// Display the trajectory
	trajectory.visual.color = { 0,0,1 };
	trajectory.add(p, 0 /*arbitrary time*/);
	if (display_trajectory)
		draw(trajectory, environment);
}

void keyframe_structure::update_picking(inputs_interaction_parameters const& inputs, environmentType& environment)
{
	// Current position of the mouse
	vec2 const& p = inputs.mouse.position.current;

	// The picking and deformation is only applied when pressing the shift key
	if (inputs.keyboard.shift)
	{
		// If the mouse is not clicked, compute a picking on the vertices of the grid
		if (!inputs.mouse.click.left)
			picking = picking_spheres(p, key_positions, 0.05f, environment.camera, environment.projection);

		// Key position translation
		if (inputs.mouse.click.left && picking.active)
		{
			vec3 new_position = picking_plane_orthogonal_to_camera(p, picking.position, environment.camera, environment.projection).position;
			key_positions[picking.index] = new_position;
		}
	}
	else
		picking.active = false;
}

void keyframe_structure::display_gui()
{
	ImGui::Checkbox("Display key positions", &display_keyposition);
	ImGui::Checkbox("Display polygon", &display_polygon);
	ImGui::Checkbox("Display trajectory", &display_trajectory);
	bool new_size = ImGui::SliderInt("Trajectory size", &trajectory_storage, 2, 500);

	if (new_size) {
		trajectory.clear();
		trajectory = trajectory_drawable(trajectory_storage);
	}
}