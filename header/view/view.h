#pragma once

#include <memory> // for shared pointers
#include <vector> // include vector
#include <string> // include strings
#include <chrono> // for flicking cursor
#include <iostream>

// libraries for interface
#include "GL/glew.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "GLFW/glfw3.h"

#include "query.h" // structure to process query

// interface configurations
struct configs {
	// main window configs
	int MAIN_WINDOW_WIDTH = 600;
	int MAIN_WINDOW_HEIGHT = 800;

	// input screen configs
	float INPUT_SCREEN_WIDTH_FACTOR = 0.85;
	float INPUT_SCREEN_HEIGHT_FACTOR = 0.2;
	float INPUT_SCREEN_X_COOR_FACTOR = 0.075;
	float INPUT_SCREEN_Y_COOR_FACTOR = 0.05;

	// error screen configs
	float ERROR_SCREEN_WIDTH_FACTOR = 0.6;
	float ERROR_SCREEN_HEIGHT_FACTOR = 0.1;
	float ERROR_SCREEN_X_COOR_FACTOR = 0.2;
	float ERROR_SCREEN_Y_COOR_FACTOR = 0.1;

	// buttons configs
	float BUTTONS_INIT_X_FACTOR = 0.075;
	float BUTTONS_INIT_Y_FACTOR = 0.3;
	float BUTTONS_WIDTH_FACTOR_FIRST = 0.09375; // width size of buttons in first part
	float BUTTONS_WIDTH_FACTOR_SECOND = 0.125; // width size of buttons in second part
	float BUTTONS_HEIGHT_FACTOR = 0.05;
	float BUTTONS_SHIFTX_FACTOR_FIRST = 0.09375 + 0.1 / 7; // horizontal seperation of buttons in first part
	float BUTTONS_SHIFTX_FACTOR_SECOND = 0.145; // horizontal seperation of buttons in second part
	float BUTTONS_SHIFTY_FACTOR = 0.075; // vertical separation of buttons
	int BUTTONS_COUNT = 62; // total number of buttons
	int BUTTONS_COUNT_FIRST = 32; // number of buttons in first part
	int BUTTONS_COUNT_SECOND = 30; // number of buttons in second part
	int BUTTONS_PER_LINE_FIRST = 8; // number of buttons per line in first part
	int BUTTONS_PER_LINE_SECOND = 6; // number of buttons per line in second part

	// type config
	float TYPE_INIT_X_FACTOR = 0.1 / 7.5; // x coordinate of where the type is shown
	float TYPE_INIT_Y_FACTOR = 0.18; // y coordinate of where the type is shown

	// expressions configs
	int MAX_EXP_ALLOWED = 5;

	// variables configs
	int MAX_VARIABLES_ALLOWED = 26;

	// matrices configs_
	int MAX_DIMENSION_ALLOWED = 6;
	int MIN_DIMENSION_ALLOWED = 1;
};

enum answerType {
	noAns = 0,
	systemAns,
	calcAns
};

class View {
public:
	// types definitions
	using sptrView = std::shared_ptr<View>;

	// contructor and destructor
	~View() = default;
	View(const View& other) = default;
	View& operator=(const View& other) = default;

	// create a single object of the class
	static sptrView createView();

	// initalize imgui and glfw
	void initImGui();
	void initGlfw();

	// create window
	void createWindow();

	// create new frame for iteration
	void newFrame();

	// rendering
	void render();

	// interface set up
	void setUpInterface();

	// clean up
	void cleanUp();

	// getter
	GLFWwindow* getWindow();

	// checkers
	bool isWindowOpen();

	// pass query to controller and process received answer
	Query& getQuery();
	void processAnswer(const Answer& answer);

private:
	// private constructor for singleton pattern
	View();

	// screen for expressions
	void displayInputScreen();
	void displayCursor();
	void displayExp();
	void displayType();
	int getTextLength(const std::string& text);
	ImVec2 getCursorPos();
	void displayInitMatrix();
	void displayMatrix(const std::vector<std::vector<std::string>>& matrix);
	void displayInitEquation();
	void printAnswer();
	void printSolutionToSystem();
	void printExpAns();

	// display buttons
	void displayButtons();
	void displayNumberKeyBoard();

	// display errors
	void displayErrors();

	// popups
	void displayTypePopup();
	void displayDimensionsPopup();
	void displayListPopup();
	void displayEquationPopup();

	// handle buttons
	void handleGeneralButton(const std::string& symbol, float width, float height);
	void handleInitButton(float width, float height);
	void handleListButton(float width, float height);
	void handleUpButton(float width, float height);
	void handleDownButton(float width, float height);
	void handleLeftButton(float width, float height);
	void handleRightButton(float width, float height);
	void handleClearButton(float width, float height);
	void handleDeleteButton(float width, float height);
	void handleTypeButton(float width, float height);
	void handleEqualButton(float width, float height);
	void handleEquationButton(float width, float height);

	// checkers
	bool isToken(const std::string& text);
	bool isModuloValid();
	bool isEligibleForInitMatrix(const std::string& symbol);
	bool isNoSolutionRow(const std::vector<float>& row);

	// getters
	std::string& getTextToUse();

	std::vector<std::string> buttons_labels_; // labels for buttons
	GLFWwindow* window_; // main window
	bool show_cursor_; // indicator wether to draw cursor on screen or not
	int cursor_pos_; // position of cursor in relation to the stored expression
	std::pair<int, int> cursor_cell_; // used for initializing matrix
	std::vector<std::string> expressions_; // stored expressions
	int cur_exp_; // current expression displayed
	bool show_type_popup_; // popup for type
	bool modulo_pressed_; // whether type modulo is picked
	bool show_dimensions_popup_; // popup to initialize matrix
	bool show_list_popup_; // popup to list all used variables
	bool init_matrix_; // start initiallizing matrix
	bool show_equation_popup_; // prompt for equations info
	bool init_system_; // start initiallizing system of equations
	bool is_ans_chosen_; // if ans is chosen for initiallizing
	std::string error_message_; // error message, if no errors then empty string
	std::vector<std::vector<std::vector<std::string>>> variables_; // list of all variables and their stored values
	int cur_available_variable_; // current available variables
	std::vector<std::vector<std::string>> equations_; // matrix for system of equations
	std::chrono::steady_clock::time_point lastToggleTime_; // time stamp of last display of cursor
	std::string type_;
	configs configs_; // configurations of interface
	Query query_;
	Answer answer_;
	int is_answer_ready_; // whether to print the answer out or not
	static sptrView view_; // singleton pattern
};
