#include "controller.h" // include header file

// contructor and destructor

Controller::Controller(): model_(Model::createModel()),
						  view_(View::createView())
{}

// main function run
void Controller::run() {
	view_->initGlfw();

	view_->createWindow();

	view_->initImGui();

	while (view_->isWindowOpen()) {
		glfwPollEvents();

		view_->newFrame();

		view_->setUpInterface();

		Query query = view_->getQuery();

		if (query.type_of_query_ != noQuery) {
			Answer ans = model_->processQuery(query);
			view_->processAnswer(ans);
		}

		view_->render();
	}

	view_->cleanUp();
}
