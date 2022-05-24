#pragma once

#include "cgp/cgp.hpp"
#include "environment_camera_head/environment_camera_head.hpp"
#include "key_positions_structure.hpp"

// The element of the GUI that are not already stored in other structures
struct gui_parameters {
	bool display_frame      = true;
	bool display_wireframe  = false;
	float speed = 1.0f;       // The velocity of the camera displacement
};



// The structure of the custom scene
struct scene_structure {
	
	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //

	cgp::mesh_drawable global_frame;          // The standard global frame
	cgp::scene_environment_basic_camera_spherical_coords environment; // Standard environment controler
	cgp::inputs_interaction_parameters inputs; // Storage for inputs status (mouse, keyboard, window dimension)

	gui_parameters gui;                       // Standard GUI element storage
	
	
	//scene_environment_camera_head environment_move; // The specific scene environment that contains a "head camera" (*)


	cgp::mesh_drawable terrain;         // The terrain loaded from an external file

	cgp::timer_interval timer; // A basic timer for the camera animation
	
	keyframe_structure keyframe;




	cgp::mesh_drawable pond;
	cgp::mesh_drawable tree;
	cgp::mesh_drawable parapet;
	cgp::mesh_drawable parapet1;
	cgp::mesh_drawable parapet2;
	cgp::mesh_drawable parapet3;
	cgp::mesh_drawable parapet4;

	std::vector<cgp::vec2> randpos;
	std::vector<float> randscale;
	std::vector<cgp::mesh> parapets;
	//cgp::mesh_drawable tree;

	// ****************************** //
	// Functions
	// ****************************** //

	void initialize();  // Standard initialization to be called before the animation loop
	void display();     // The frame display to be called within the animation loop
	void display_gui(); // The display of the GUI, also called within the animation loop
	void scene_structure::update_camera();


	void mouse_move();
};





