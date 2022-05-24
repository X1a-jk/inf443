#include "scene.hpp"

#include "terrain.hpp"
#include "tree.hpp"

#include "interpolation.hpp"

using namespace cgp;



//mesh_drawable tree;
std::vector<cgp::vec3> tree_position;

/*
void scene_structure::update_camera()
{
	inputs_keyboard_parameters const& keyboard = inputs.keyboard;
	camera_head& camera = environment_move.camera;

	// The camera moves forward all the time
	//   We consider in this example a constant velocity, so the displacement is: velocity * dt * front-camera-vector
	float const dt = timer.update();
	vec3 const forward_displacement = gui.speed * 0.1f * dt * camera.front();
	camera.position_camera += forward_displacement;

	// The camera rotates if we press on the arrow keys
	//  The rotation is only applied to the roll and pitch degrees of freedom.
	float const pitch = 0.5f; // speed of the pitch
	float const roll = 0.7f; // speed of the roll
	if (keyboard.up)
		camera.manipulator_rotate_roll_pitch_yaw(0, -pitch * dt, 0);
	if (keyboard.down)
		camera.manipulator_rotate_roll_pitch_yaw(0, pitch * dt, 0);
	if (keyboard.right)
		camera.manipulator_rotate_roll_pitch_yaw(roll * dt, 0, 0);
	if (keyboard.left)
		camera.manipulator_rotate_roll_pitch_yaw(-roll * dt, 0, 0);
}
*/



void scene_structure::initialize()
{
	// Basic set-up
	// ***************************************** //

	global_frame.initialize(mesh_primitive_frame(), "Frame");
	environment.camera.axis = camera_spherical_coordinates_axis::z;
	environment.camera.look_at({ 15.0f,6.0f,6.0f }, { 0,0,0 });
	//environment_move.camera.position_camera = { 0.5f, 0.5f, -2.0f };
	//environment_move.camera.manipulator_rotate_roll_pitch_yaw(0, 0, Pi / 2.0f);

	int N_terrain_samples = 100;
	float terrain_length = 40;
	mesh const terrain_mesh = create_terrain_mesh(N_terrain_samples, terrain_length);
	
	terrain.initialize(terrain_mesh, "terrain");
	//terrain.shading.color = { 0.6f,0.85f,0.5f };
	terrain.shading.phong.specular = 0.0f; // non-specular terrain material

	

	mesh const parapet_mesh = create_parapet(0.7f);
	parapet.initialize(parapet_mesh, "parapet");
	//tree_position = generate_positions_on_terrain(10, terrain_length/1.5);

	
	GLuint const texture_image_id = opengl_load_texture_image("assets/grass.jpg",
		GL_CLAMP_TO_BORDER,
		GL_CLAMP_TO_BORDER);

	terrain.texture = texture_image_id;
	
	mesh const pond_mesh = create_pond_mesh(N_terrain_samples, terrain_length);
	pond.initialize(pond_mesh, "pond");
	//pond.shading.color = { 0.0f,0.0f,1.0f };
	pond.shading.phong.specular = 0.0f;

	GLuint const pond_image_id = opengl_load_texture_image("assets/pond.jpg",
		GL_CLAMP_TO_BORDER,
		GL_CLAMP_TO_BORDER);

	pond.texture = pond_image_id;

	/*mesh const tree_mesh = create_tree();
	tree.initialize(tree_mesh, "tree");
	
	*/


	mesh tree_mesh = mesh_load_file_obj("assets/lowpolytree.obj");
	tree.initialize(tree_mesh, "tree");
	tree_position = generate_positions_trees(10, terrain_length);
	
	int len = tree_position.size();
	for (int i = 0; i < len; i++) {
		float ku = rand_interval(-2.0f, 2.0f);
		float kv = rand_interval(-2.0f, 2.0f);
		randpos.push_back({ ku,kv });
		float ks= rand_interval(0.5f, 1.2f);
		randscale.push_back(ks);
	}


	mesh const parapet_1 = create_parapet(1.4f); //右下
	parapet1.initialize(parapet_1, "parapet");
	parapet1.transform.translation = vec3({ 8.0f,10.0f,0.0f });
	parapet1.transform.rotation = rotation_transform::from_axis_angle({ 0,0,1 }, 3.14f / 2);
	//parapets.push_back(parapet_1);

	mesh const parapet_2 = create_parapet(0.9f); //左下
	parapet2.initialize(parapet_2, "parapet");
	parapet2.transform.translation = vec3({ 16.0f,-14.8f,0.0f });
	parapet2.transform.rotation = rotation_transform::from_axis_angle({ 0,0,1 }, 3.14f / 3);
	//parapets.push_back(parapet_2);

	mesh const parapet_3 = create_parapet(0.8f);  //右上
	parapet3.initialize(parapet_3, "parapet");
	parapet3.transform.translation = vec3({ -12.0f,13.6f,0.0f });
	parapet3.transform.rotation = rotation_transform::from_axis_angle({ 0,0,1 }, -3.14f / 2);
	//parapets.push_back(parapet_3);


	mesh const parapet_4 = create_parapet(1.2f); //左上
	parapet4.initialize(parapet_4, "parapet");
	parapet4.transform.translation = vec3({ -10.5f,-0.6f,2.2f });
	parapet4.transform.rotation = rotation_transform::from_axis_angle({ 0,0,1 }, 0.0f);
	//parapets.push_back(parapet_4);

	
	buffer<vec3> key_positions =
	{ {20.0f,-20.0f,0.0f}, {18.0f,-14.0f,0.0f}, {14.5f,-14.0f,0.2f}, {0.0f,-12.0f,3.0f}, {-9.0f,-9.0f,6.8f}, {-8.0f,3.0f,2.4f}, {-16.0f,13.0f,0.5f}, {-10.0f,12.0f,0.5f}, {0.0f,14.0f,0.5f}, {10.0f,12.0f,0.5f}, {20.0f,20.0f,0.5f}, {20.0f,0.0f,0.5f} };

	buffer<float> key_times =
	{ 0.0f, 1.5f, 3.0f, 4.5f, 6.0f, 7.5f, 8.75f, 10.5f, 12.0f, 13.5f, 15.0f, 18.0f };

	keyframe.initialize(key_positions, key_times);

	int N = key_times.size();
	timer.t_min = key_times[1];
	timer.t_max = key_times[N - 2];
	timer.t = timer.t_min;
	
	
}





void scene_structure::display()
{

	// Basic elements of the scene
	environment.light = environment.camera.position();
	if (gui.display_frame)
		draw(global_frame, environment);

	
	draw(pond, environment);
	draw(terrain, environment);
	//draw(tree, environment);
	draw(parapet1, environment);
	draw(parapet2, environment);
	draw(parapet3, environment);
	draw(parapet4, environment);

	int i = 0;
	for (vec3 pos : tree_position) {
		
		mesh const tree_i = mesh_load_file_obj("assets/lowpolytree.obj");
		tree.initialize(tree_i, "tree");
		//parapet.transform.translation = pos;
		tree.transform.rotation = rotation_transform::from_axis_angle({ 1,0,0 }, 3.14f / 2);
		tree.transform.scaling = randscale[i];
		tree.shading.color = { 78.0f / 255,137.0f / 255,30.0f / 255 };
		//tree.transform.translation = pos;
		
		
		vec3 adjust = { randpos[i][0],randpos[i][1],randscale[i]*1.8f};
		tree.transform.translation = pos+adjust;
		i++;
		draw(tree, environment);
	}
	
	
	timer.update();
	float t = timer.t;
	if (t < timer.t_min + 0.1f)
		keyframe.trajectory.clear();


	keyframe.display_key_positions(environment);

	vec3 p = interpolation(t, keyframe.key_positions, keyframe.key_times);

	keyframe.display_current_position(p, environment);
	
	
	if (gui.display_wireframe)
		draw_wireframe(terrain, environment);

}






void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);
	ImGui::Checkbox("Wireframe", &gui.display_wireframe);

	
	ImGui::SliderFloat("Time", &timer.t, timer.t_min, timer.t_max);
	ImGui::SliderFloat("Time scale", &timer.scale, 0.0f, 2.0f);

	

	keyframe.display_gui();
	
}


void scene_structure::mouse_move()
{
	// Handle the picking (displacement of the position using mouse drag)
	keyframe.update_picking(inputs, environment);
}


/*
void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);
	ImGui::SliderFloat("Speed", &gui.speed, 0.2f, 5.0f); // Set the camera velocity
}
*/
