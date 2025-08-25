#pragma once // header file included only once

#include <memory> // for shared pointers

#include "GL/glew.h"
#include "GLFW/glfw3.h" // include glfw

#include "model.h" // include Model
#include "view.h" // include View
#include "query.h"

class Controller {
public:
	// types definitions

	using sptrModel = std::shared_ptr<Model>;
	using sptrView = std::shared_ptr<View>;

	// constructor and destructor

	Controller();
	~Controller() = default;
	Controller(const Controller& other) = default;
	Controller& operator=(const Controller& other) = default;

	// main function run

	void run();

private:
	sptrModel model_; // pointer to the only object of class Model
	sptrView view_; // pointer to the only object of class View
};
