#include <ctime>
#include <iostream>
#include <fstream> // include files handlings
#include <string>
#include <cmath>

#include "imgui.h" // include imgui

#include "view.h" // include header file

// initialize static member
View::sptrView View::view_ = nullptr;

// create a single object of the class
View::sptrView View::createView() {
	if (view_ == nullptr) {
		view_ = sptrView(new View());
	}

	return view_;
}

// initialize imgui and glfw
void View::initImGui() {
	ImGui::CreateContext();
	static const ImWchar ranges[] = {
		0x0020, 0x00FF,
		0x2190, 0x2193,
		0,
	};
	ImGuiIO& io = ImGui::GetIO();
	ImFont* font = io.Fonts->AddFontFromFileTTF("../arial.ttf", 15, NULL, ranges);
	io.FontDefault = font;
	ImGui_ImplGlfw_InitForOpenGL(window_, true);
	ImGui_ImplOpenGL3_Init();
}

void View::initGlfw() {
	glfwInit();
}

// create window
void View::createWindow() {
	window_ = glfwCreateWindow(configs_.MAIN_WINDOW_WIDTH, configs_.MAIN_WINDOW_HEIGHT, "Matrix Calculator", nullptr, nullptr);
	glfwMakeContextCurrent(window_);
}

// create new frame for iteration
void View::newFrame() {
	ImGui_ImplGlfw_NewFrame();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();
}

// rendering
void View::render() {
	glClear(GL_COLOR_BUFFER_BIT);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(window_);
}

// set up interface
void View::setUpInterface() {
	int width, height;
	glfwGetWindowSize(window_, &width, &height);

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(width, height));

	ImGui::Begin("Main Window", nullptr, ImGuiWindowFlags_NoTitleBar |
										 ImGuiWindowFlags_NoResize |
										 ImGuiWindowFlags_NoMove |
										 ImGuiWindowFlags_NoScrollbar |
										 ImGuiWindowFlags_NoScrollWithMouse |
										 ImGuiWindowFlags_NoCollapse |
										 ImGuiWindowFlags_NoBringToFrontOnFocus);

	displayInputScreen();
	displayButtons();
	displayTypePopup();
	displayDimensionsPopup();
	displayListPopup();
	displayEquationPopup();
	displayErrors();

	ImGui::End();
}

// clean up
void View::cleanUp() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window_);
	glfwTerminate();
}

// getters
GLFWwindow* View::getWindow() {
	return window_;
}

// checkers
bool View::isWindowOpen() {
	return !glfwWindowShouldClose(window_);
}

// pass query to controller and process received answer
Query& View::getQuery() {
	return query_;
}

// handle answer
void View::processAnswer(const Answer& answer) {
	query_.type_of_query_ = noQuery;
	error_message_ = answer.error_message_;

	if (error_message_ != "") {
		return;
	}

	if (init_matrix_) {
		if (error_message_ == "") {
			init_matrix_ = false;
			is_ans_chosen_ = false;
			cursor_cell_.first = 0;
			cursor_cell_.second = 0;
			cursor_pos_ = expressions_[cur_exp_].size();
			cur_available_variable_ = (cur_available_variable_ + 1) % configs_.MAX_VARIABLES_ALLOWED;
		}

		return;
	}

	if (init_system_) {
		if (error_message_ == "") {
			init_system_ = false;
			cursor_cell_.first = 0;
			cursor_cell_.second = 0;
			cursor_pos_ = expressions_[cur_exp_].size();
			answer_ = answer;
			is_answer_ready_ = systemAns;
		}

		return;
	}

	answer_ = answer;
	is_answer_ready_ = calcAns;

	return;
}

// private contructor for singleton patter
View::View(): window_(nullptr),
			  show_cursor_(true),
			  cursor_pos_(0),
			  cursor_cell_(std::make_pair(0, 0)),
			  cur_exp_(0),
			  type_("real"),
			  show_type_popup_(false),
			  modulo_pressed_(false),
			  error_message_(""),
			  show_dimensions_popup_(false),
			  show_list_popup_(false),
			  cur_available_variable_(0),
			  init_matrix_(false),
			  show_equation_popup_(false),
			  init_system_(false),
			  is_ans_chosen_(false),
			  is_answer_ready_(noAns)
{
	variables_.resize(configs_.MAX_VARIABLES_ALLOWED);

	expressions_.push_back("");

	buttons_labels_.resize(configs_.BUTTONS_COUNT);

	// set up labels for buttons
	std::ifstream labels("../labels.txt");
	for (int i = 0; i < buttons_labels_.size(); ++i) {
		labels >> buttons_labels_[i];
	}
}

// screen for expressions
void View::displayInputScreen() {
	int width, height;
	glfwGetWindowSize(window_, &width, &height);
	ImGui::SetNextWindowSize(ImVec2(width * configs_.INPUT_SCREEN_WIDTH_FACTOR, height * configs_.INPUT_SCREEN_HEIGHT_FACTOR));
	ImGui::SetNextWindowPos(ImVec2(width * configs_.INPUT_SCREEN_X_COOR_FACTOR, height * configs_.INPUT_SCREEN_Y_COOR_FACTOR));

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(255, 255, 255, 255));

	ImGui::Begin("Input screen", nullptr, ImGuiWindowFlags_NoTitleBar |
										  ImGuiWindowFlags_NoResize |
										  ImGuiWindowFlags_NoMove |
										  ImGuiWindowFlags_NoScrollbar |
										  ImGuiWindowFlags_NoScrollWithMouse |
										  ImGuiWindowFlags_NoCollapse);

	displayCursor();
	displayExp();
	displayInitMatrix();
	displayInitEquation();
	printAnswer();
	displayType();

	ImGui::End();

	ImGui::PopStyleColor();
}

// cursor
void View::displayCursor() {
	if (is_answer_ready_ == systemAns) {
		return;
	}

	ImVec2 cursorPos = ImGui::GetCursorScreenPos();
	auto curTime = std::chrono::steady_clock::now();
	auto interval = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - lastToggleTime_);

	if (interval.count() > 500) {
		show_cursor_ = !show_cursor_;
		lastToggleTime_ = curTime;
	}

	// change cursor position according to text and shift
	ImVec2 shift = getCursorPos();
	cursorPos.x += shift.x;
	if (init_matrix_ || init_system_) {
		cursorPos.y += shift.y;
	}

	if (show_cursor_) {
		ImDrawList* list = ImGui::GetWindowDrawList();
		list->AddLine(cursorPos, ImVec2(cursorPos.x, cursorPos.y + ImGui::GetTextLineHeight()), IM_COL32(0, 0, 0, 255), 2);
	}
}

void View::displayExp() {
	if (init_matrix_ || init_system_ || is_answer_ready_ == systemAns) {
		return;
	}

	ImVec4 color = ImVec4(0, 0, 0, 255);

	ImGui::TextColored(color, expressions_[cur_exp_].c_str());
}

void View::displayType() {
	int width, height;
	glfwGetWindowSize(window_, &width, &height);
	float x_coor = configs_.TYPE_INIT_X_FACTOR * width;
	float y_coor = configs_.TYPE_INIT_Y_FACTOR * height;

	ImVec4 color = ImVec4(0, 0, 0, 255);
	ImGui::SetCursorPos(ImVec2(x_coor, y_coor));
	ImGui::TextColored(color, type_.c_str());
}

// calculate cursor postion
ImVec2 View::getCursorPos() {
	if (init_matrix_ || init_system_) {
		if (is_ans_chosen_) {
			int xcoor = ImGui::CalcTextSize("ans").x;
			int ycoor = 4 + ImGui::GetTextLineHeight();
			return ImVec2(xcoor, ycoor);
		}

		const std::vector<std::vector<std::string>>& matrix = init_matrix_ ? variables_[cur_available_variable_] : equations_;

		int row = cursor_cell_.first;
		int col = cursor_cell_.second;
		int xcoor = ImGui::CalcTextSize("0").x * (col + 1);
		int ycoor = (4 + ImGui::GetTextLineHeight()) * (row + 1);

		int dist = 0;
		for (int i = 0; i < col; ++i) {
			dist += ImGui::CalcTextSize(matrix[row][i].c_str()).x;
		}

		std::string text_before_cursor = "";
		for (int i = 0; i < cursor_pos_; ++i) {
			text_before_cursor += matrix[row][col][i];
		}
		xcoor += dist + ImGui::CalcTextSize(text_before_cursor.c_str()).x;

		return ImVec2(xcoor, ycoor);
	}

	std::string text_before_cursor = "";
	for (int i = 0; i < cursor_pos_; ++i) {
		text_before_cursor += expressions_[cur_exp_][i];
	}

	return ImGui::CalcTextSize(text_before_cursor.c_str());
}

// display matrix for initializing variable
void View::displayInitMatrix() {
	if (!init_matrix_) {
		return;
	}

	ImVec4 color = ImVec4(0, 0, 0, 255);

	std::string symbol(1, char(cur_available_variable_ + 'A'));
	ImGui::TextColored(color, (symbol + ":").c_str());

	if (is_ans_chosen_) {
		ImGui::TextColored(color, "ans");
		return;
	}

	displayMatrix(variables_[cur_available_variable_]);
}

// display a given matrix
void View::displayMatrix(const std::vector<std::vector<std::string>>& matrix) {
	ImVec4 color = ImVec4(0, 0, 0, 255);

	if (show_list_popup_) {
		color = ImVec4(255, 255, 255, 255);
	}

	ImGui::NewLine();
	ImGui::SameLine();

	for (int i = 0; i < matrix.size(); ++i) {
		for (int j = 0; j < matrix[i].size(); ++j) {
			ImGui::SameLine();
			std::string num = matrix[i][j];
			if (num != "0") {
				num.erase(num.find_last_not_of('0') + 1, std::string::npos);
				num.erase(num.find_last_not_of('.') + 1, std::string::npos);
			}
			ImGui::TextColored(color, num.c_str());
		}
		ImGui::NewLine();
	}
}

// display matrix to initialize system of linear equations
void View::displayInitEquation() {
	if (!init_system_) {
		return;
	}

	ImGui::NewLine();
	displayMatrix(equations_);
}

// print answer
void View::printAnswer() {
	if (!is_answer_ready_ || init_matrix_) return;

	if (is_answer_ready_ == systemAns) {
		printSolutionToSystem();
	}

	if (is_answer_ready_ == calcAns) {
		printExpAns();
	}
}

// print solution to system of linear equations
void View::printSolutionToSystem() {
	ImVec4 color = ImVec4(0, 0, 0, 255);
	std::vector<bool> entries;
	entries.resize(answer_.ans_matrix_[0].size() - 1);

	// print out no solution
	for (int i = 0; i < answer_.ans_matrix_.size(); ++i) {
		if (isNoSolutionRow(answer_.ans_matrix_[i])) {
			ImGui::TextColored(color, "No solution");
			return;
		}
	}

	// find leading entries
	for (int i = 0; i < answer_.ans_matrix_.size(); ++i) {
		for (int j = 0; j < entries.size(); ++j) {
			if (answer_.ans_matrix_[i][j]) {
				entries[j] = true;
				break;
			}
		}
	}

	// print the answer to the system out otherwise
	int cur_line = 0;
	bool has_no_free_entry = true;
	int free_var = entries.size();
	for (int i = 0; i < entries.size(); ++i) {
		std::string line = "x" + std::to_string(i + 1) + " = ";
		if (entries[i]) {
			std::string rhs = "";
			for (int j = i + 1; j < answer_.ans_matrix_[cur_line].size() - 1; ++j) {
				if (answer_.ans_matrix_[cur_line][j] != 0.0) {
					has_no_free_entry = false;
					rhs += answer_.ans_matrix_[cur_line][j] < 0 ? "+" : "-";
					std::string val = std::to_string(std::abs(answer_.ans_matrix_[cur_line][j]));
					val.erase(val.find_last_not_of('0') + 1, std::string::npos);
					val.erase(val.find_last_not_of('.') + 1, std::string::npos);
					if (val != "1") {
						rhs += val + "*";
					}
					rhs += "x" + std::to_string(j + 1);
				}
			}
			std::string val = std::to_string(answer_.ans_matrix_[cur_line][free_var]);
			val.erase(val.find_last_not_of('0') + 1, std::string::npos);
			val.erase(val.find_last_not_of('.') + 1, std::string::npos);
			line += ((answer_.ans_matrix_[cur_line][free_var] != 0.0 || has_no_free_entry ? val : "")) + rhs;
			has_no_free_entry = true;
			++cur_line;
		}
		else {
			line += "x" + std::to_string(i + 1);
		}

		ImGui::TextColored(color, line.c_str());
	}
}

// print answer to expression
void View::printExpAns() {
	ImVec4 color = ImVec4(0, 0, 0, 255);

	if (answer_.is_ans_number_) {
		std::string ans = std::to_string(answer_.ans_float_);
		ans.erase(ans.find_last_not_of('0') + 1, std::string::npos);
		ans.erase(ans.find_last_not_of('.') + 1, std::string::npos);
		ImGui::NewLine();
		ImGui::TextColored(color, ans.c_str());
	}
	else {
		std::vector<std::vector<std::string>> ans;
		ans.resize(answer_.ans_matrix_.size());
		for (int i = 0; i < ans.size(); ++i) {
			ans[i].resize(answer_.ans_matrix_[0].size());
		}
		for (int i = 0; i < ans.size(); ++i) {
			for (int j = 0; j < ans[i].size(); ++j) {
				ans[i][j] = std::to_string(answer_.ans_matrix_[i][j]);
			}
		}

		displayMatrix(ans);
	}
}

// display buttons
void View::displayButtons() {
	int width, height;
	glfwGetWindowSize(window_, &width, &height);
	float curx = configs_.BUTTONS_INIT_X_FACTOR * width;
	float cury = configs_.BUTTONS_INIT_Y_FACTOR * height;
	float buttonWidth1 = configs_.BUTTONS_WIDTH_FACTOR_FIRST * width;
	float buttonWidth2 = configs_.BUTTONS_WIDTH_FACTOR_SECOND * width;
	float buttonHeight = configs_.BUTTONS_HEIGHT_FACTOR * height;
	float shiftx1 = configs_.BUTTONS_SHIFTX_FACTOR_FIRST * width;
	float shiftx2 = configs_.BUTTONS_SHIFTX_FACTOR_SECOND * width;
	float shifty = configs_.BUTTONS_SHIFTY_FACTOR * height;

	for (int i = 0; i < configs_.BUTTONS_COUNT_FIRST; ++i) {
		ImGui::SetCursorPos(ImVec2(curx, cury));

		if (buttons_labels_[i] == "ini") {
			handleInitButton(buttonWidth1, buttonHeight);
		}
		else if (buttons_labels_[i] == "list") {
			handleListButton(buttonWidth1, buttonHeight);
		}
		else if (buttons_labels_[i] == "type") {
			handleTypeButton(buttonWidth1, buttonHeight);
		}
		else {
			handleGeneralButton(buttons_labels_[i], buttonWidth1, buttonHeight);
		}

		curx += shiftx1;
		if ((i + 1) % configs_.BUTTONS_PER_LINE_FIRST == 0) {
			curx = configs_.BUTTONS_INIT_X_FACTOR * width;
			cury += shifty;
		}
	}

	for (int i = 0; i < configs_.BUTTONS_COUNT_SECOND; ++i) {
		ImGui::SetCursorPos(ImVec2(curx, cury));

		int cur_button = i + configs_.BUTTONS_COUNT_FIRST;
		std::string cur_button_symbol = buttons_labels_[cur_button];

		if (cur_button_symbol == "up") {
			handleUpButton(buttonWidth2, buttonHeight);
		}
		else if (cur_button_symbol == "down") {
			handleDownButton(buttonWidth2, buttonHeight);
		}
		else if (cur_button_symbol == "left") {
			handleLeftButton(buttonWidth2, buttonHeight);
		}
		else if (cur_button_symbol == "right") {
			handleRightButton(buttonWidth2, buttonHeight);
		}
		else if (cur_button_symbol == "clr") {
			handleClearButton(buttonWidth2, buttonHeight);
		}
		else if (cur_button_symbol == "del") {
			handleDeleteButton(buttonWidth2, buttonHeight);
		}
		else if (cur_button_symbol == "=") {
			handleEqualButton(buttonWidth2, buttonHeight);
		}
		else if (cur_button_symbol == "eq") {
			handleEquationButton(buttonWidth2, buttonHeight);
		}
		else {
			handleGeneralButton(cur_button_symbol, buttonWidth2, buttonHeight);
		}

		curx += shiftx2;
		if ((i + 1) % configs_.BUTTONS_PER_LINE_SECOND == 0) {
			curx = configs_.BUTTONS_INIT_X_FACTOR * width;
			cury += shifty;
		}
	}
}

// display numbers pad for modulo prompt
void View::displayNumberKeyBoard() {
	ImGui::NewLine();
	for (int i = 1; i <= 9; ++i) {
		if (i % 3 == 1) {
			ImGui::InvisibleButton("spacebar1", ImVec2(40, 10));
			ImGui::SameLine();
		}
		if (ImGui::Button(std::to_string(i).c_str())) {
			type_ += std::to_string(i);
		}
		ImGui::SameLine();
		if (i % 3 == 0) {
			ImGui::NewLine();
		}
	}
	ImGui::InvisibleButton("spacebar1", ImVec2(64, 10));
	ImGui::SameLine();
	if (ImGui::Button("0")) {
		type_ += "0";
	}
	ImGui::InvisibleButton("spacebar2", ImVec2(50, 10));
	ImGui::SameLine();
	if (ImGui::Button("enter")) {
		if (!isModuloValid()) {
			error_message_ = "invalid modulo";
		}
		else {
			modulo_pressed_ = false;
		}
	}
}

// display errors
void View::displayErrors() {
	if (error_message_.length()) {
		ImGui::OpenPopup("error screen");
	}

	int width, height;
	glfwGetWindowSize(window_, &width, &height);
	float error_width = width * configs_.ERROR_SCREEN_WIDTH_FACTOR;
	float error_height = height * configs_.ERROR_SCREEN_HEIGHT_FACTOR;
	float xcoor = width * configs_.ERROR_SCREEN_X_COOR_FACTOR;
	float ycoor = height * configs_.ERROR_SCREEN_Y_COOR_FACTOR;

	ImGui::SetNextWindowSize(ImVec2(error_width, error_height));
	ImGui::SetNextWindowPos(ImVec2(xcoor, ycoor));

	if (ImGui::BeginPopupModal("error screen", nullptr, ImGuiWindowFlags_NoScrollbar|
										  				ImGuiWindowFlags_NoMove |
										  				ImGuiWindowFlags_NoResize)) {

		ImGui::Text(error_message_.c_str());

		if (error_message_ == "invalid modulo") {
			show_type_popup_ = false;
			modulo_pressed_ = false;
		}

		if (ImGui::Button("ok")) {
			if (error_message_ == "invalid modulo") {
				show_type_popup_ = true;
				modulo_pressed_ = true;
				type_ = "modulo ";
			}

			error_message_ = "";
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

// popups
void View::displayTypePopup() {
	if (show_type_popup_) {
		ImGui::OpenPopup("Type");
	}

	if (ImGui::BeginPopupModal("Type", nullptr, ImGuiWindowFlags_NoMove |
												ImGuiWindowFlags_NoResize |
												ImGuiWindowFlags_AlwaysAutoResize))
	{
		if (ImGui::Button("real")) {
			type_ = "real";
			show_type_popup_ = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("complex")) {
			type_ = "complex";
			show_type_popup_ = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("modulo") && !modulo_pressed_) {
			type_ = "modulo ";
			modulo_pressed_ = true;
		}

		ImGui::NewLine();
		if (modulo_pressed_) {
			displayNumberKeyBoard();
			if (!modulo_pressed_) {
				show_type_popup_ = false;
				ImGui::CloseCurrentPopup();
			}
		}

		ImGui::EndPopup();
	}
}

void View::displayDimensionsPopup() {
	if (show_dimensions_popup_) {
		ImGui::OpenPopup("Dimensions");
	}

	if (ImGui::BeginPopupModal("Dimensions", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		static int row = 1;
		static int col = 1;

		ImGui::Text("row: ");
		ImGui::InputInt("##row", &row);

		if (row < configs_.MIN_DIMENSION_ALLOWED) row = configs_.MIN_DIMENSION_ALLOWED;
		if (row > configs_.MAX_DIMENSION_ALLOWED) row = configs_.MAX_DIMENSION_ALLOWED;

		ImGui::Text("column: ");
		ImGui::InputInt("##column", &col);

		if (col < configs_.MIN_DIMENSION_ALLOWED) col = configs_.MIN_DIMENSION_ALLOWED;
		if (col > configs_.MAX_DIMENSION_ALLOWED) col = configs_.MAX_DIMENSION_ALLOWED;

		if (ImGui::Button("enter")) {
			variables_[cur_available_variable_].resize(row);
			for (int i = 0; i < row; ++i) {
				variables_[cur_available_variable_][i].resize(col);
				for (int j = 0; j < col; ++j) {
					variables_[cur_available_variable_][i][j] = "0";
				}
			}

			cursor_pos_ = 1;
			show_dimensions_popup_ = false;
			init_matrix_ = true;

			ImGui::CloseCurrentPopup();
		}

		if (ImGui::Button("ans")) {
			is_ans_chosen_ = true;
			show_dimensions_popup_ = false;
			init_matrix_ = true;
			cursor_pos_ = 3;

			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void View::displayListPopup() {
	if (!show_list_popup_) {
		return;
	}

	ImGui::Begin("List of matrices", nullptr, ImGuiWindowFlags_NoCollapse);

	int i = 0;
	while (variables_[i].size() != 0) {
		std::string symbol(1, char(i + 'A'));
		ImGui::Text(symbol.c_str());
		displayMatrix(variables_[i++]);
		ImGui::Separator();
	}

	if (ImGui::Button("close")) {
		show_list_popup_ = false;
	}

	ImGui::End();
}

void View::displayEquationPopup() {
	if (show_equation_popup_) {
		ImGui::OpenPopup("Equations");
	}

	if (ImGui::BeginPopupModal("Equations", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		static int no_of_unknowns = 1;
		static int no_of_equations = 1;

		ImGui::Text("No of unknowns: ");
		ImGui::InputInt("##no_of_unknowns", &no_of_unknowns);

		if (no_of_unknowns < configs_.MIN_DIMENSION_ALLOWED) no_of_unknowns = configs_.MIN_DIMENSION_ALLOWED;
		if (no_of_unknowns > configs_.MAX_DIMENSION_ALLOWED) no_of_unknowns = configs_.MAX_DIMENSION_ALLOWED;

		ImGui::Text("No of equations: ");
		ImGui::InputInt("##no_of_equations", &no_of_equations);

		if (no_of_equations < configs_.MIN_DIMENSION_ALLOWED) no_of_equations = configs_.MIN_DIMENSION_ALLOWED;
		if (no_of_equations > configs_.MAX_DIMENSION_ALLOWED) no_of_equations = configs_.MAX_DIMENSION_ALLOWED;

		if (ImGui::Button("enter")) {
			equations_.resize(no_of_equations);
			for (int i = 0; i < no_of_equations; ++i) {
				equations_[i].resize(no_of_unknowns + 1);
				for (int j = 0; j <= no_of_unknowns; ++j) {
					equations_[i][j] = "0";
				}
			}

			show_equation_popup_ = false;
			init_system_ = true;
			cursor_pos_ = 1;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

// handle buttons

void View::handleInitButton(float width, float height) {
	if (ImGui::Button("ini", ImVec2(width, height))) {
		if (init_system_ || init_matrix_ || is_answer_ready_ == systemAns) {
			return;
		}

		show_dimensions_popup_ = true;
	}
}

void View::handleListButton(float width, float height) {
	if (ImGui::Button("list", ImVec2(width, height))) {
		if (init_system_ || is_answer_ready_ == systemAns) {
			return;
		}

		show_list_popup_ = true;
	}
}

void View::handleGeneralButton(const std::string& symbol, float width, float height) {
	if (ImGui::Button(symbol.c_str(), ImVec2(width, height))) {
		if ((init_matrix_ || init_system_) && !isEligibleForInitMatrix(symbol)) {
			return;
		}

		if (is_answer_ready_ == systemAns) {
			return;
		}

		std::string& text_to_use = getTextToUse();
		std::string new_exp = "";
		for (int i = 0; i < cursor_pos_; ++i) {
			new_exp += text_to_use[i];
		}

		new_exp += symbol;

		for (int i = cursor_pos_; i < text_to_use.length(); ++i) {
			new_exp += text_to_use[i];
		}

		text_to_use = new_exp;

		cursor_pos_ += symbol.length();
	}
}

void View::handleUpButton(float width, float height) {
	if (ImGui::Button("\u2191", ImVec2(width, height))) {
		if (is_answer_ready_ == systemAns) {
			return;
		}

		if (init_matrix_) {
			if (cursor_cell_.first > 0) {
				--cursor_cell_.first;
				int row = cursor_cell_.first;
				int col = cursor_cell_.second;
				cursor_pos_ = variables_[cur_available_variable_][row][col].length();
			}

			return;
		}

		if (init_system_) {
			if (cursor_cell_.first > 0) {
				--cursor_cell_.first;
				int row = cursor_cell_.first;
				int col = cursor_cell_.second;
				cursor_pos_ = equations_[row][col].length();
			}

			return;
		}

		if (cur_exp_) {
			--cur_exp_;
			cursor_pos_ = expressions_[cur_exp_].length();
			is_answer_ready_ = noAns;
		}
	}
}

void View::handleDownButton(float width, float height) {
	if (ImGui::Button("\u2193", ImVec2(width, height))) {
		if (is_answer_ready_ == systemAns) {
			return;
		}

		if (init_matrix_) {
			if (cursor_cell_.first < variables_[cur_available_variable_].size() - 1) {
				++cursor_cell_.first;
				int row = cursor_cell_.first;
				int col = cursor_cell_.second;
				cursor_pos_ = variables_[cur_available_variable_][row][col].length();
			}

			return;
		}

		if (init_system_) {
			if (cursor_cell_.first < equations_.size() - 1) {
				++cursor_cell_.first;
				int row = cursor_cell_.first;
				int col = cursor_cell_.second;
				cursor_pos_ = equations_[row][col].length();
			}

			return;
		}

		if (expressions_.size() < configs_.MAX_EXP_ALLOWED && cur_exp_ == expressions_.size() - 1) {
			expressions_.push_back("");
			++cur_exp_;
			cursor_pos_ = expressions_[cur_exp_].length();
			is_answer_ready_ = noAns;

			return;
		}

		if (expressions_.size() == configs_.MAX_EXP_ALLOWED && cur_exp_ == expressions_.size() - 1) {
			for (int i = 0; i < expressions_.size() - 1; ++i) {
				expressions_[i] = expressions_[i + 1];
			}

			expressions_[configs_.MAX_EXP_ALLOWED - 1] = "";
			cursor_pos_ = expressions_[cur_exp_].length();
			is_answer_ready_ = noAns;

			return;
		}

		++cur_exp_;
		cursor_pos_ = expressions_[cur_exp_].length();
		is_answer_ready_ = noAns;
	}
}

void View::handleLeftButton(float width, float height) {
	if (ImGui::Button("\u2190", ImVec2(width, height))) {
		if (is_answer_ready_ == systemAns) {
			return;
		}

		if (init_matrix_) {
			int row = cursor_cell_.first;
			int col = cursor_cell_.second;
			--cursor_pos_;
			if (cursor_pos_ < 0 && col > 0) {
				--cursor_cell_.second;
				cursor_pos_ = variables_[cur_available_variable_][row][col - 1].size();
			}
			else if (!col) {
				cursor_pos_ = 0;
			}

			return;
		}

		if (init_system_) {
			int row = cursor_cell_.first;
			int col = cursor_cell_.second;
			--cursor_pos_;
			if (cursor_pos_ < 0 && col > 0) {
				--cursor_cell_.second;
				cursor_pos_ = equations_[row][col - 1].size();
			}
			else if (!col) {
				cursor_pos_ = 0;
			}

			return;
		}

		if (cursor_pos_) {

			std::string text_to_skip(1, expressions_[cur_exp_][--cursor_pos_]);

			while (!isToken(text_to_skip)) {
				text_to_skip = expressions_[cur_exp_][--cursor_pos_] + text_to_skip;
			}
		}
	}
}

void View::handleRightButton(float width, float height) {
	if (ImGui::Button("\u2192", ImVec2(width, height))) {
		if (is_answer_ready_ == systemAns) {
			return;
		}

		if (init_matrix_) {
			int row = cursor_cell_.first;
			int col = cursor_cell_.second;
			++cursor_pos_;
			if (cursor_pos_ > variables_[cur_available_variable_][row][col].size() && col < variables_[cur_available_variable_][row].size() - 1) {
				++cursor_cell_.second;
				cursor_pos_ = variables_[cur_available_variable_][row][col + 1].size();
			}
			else if (col == variables_[cur_available_variable_][row].size() - 1) {
				--cursor_pos_;
			}

			return;
		}

		if (init_system_) {
			int row = cursor_cell_.first;
			int col = cursor_cell_.second;
			++cursor_pos_;
			if (cursor_pos_ > equations_[row][col].size() && col < equations_[row].size() - 1) {
				++cursor_cell_.second;
				cursor_pos_ = equations_[row][col + 1].size();
			}
			else if (col == equations_[row].size() - 1) {
				--cursor_pos_;
			}

			return;
		}

		if (cursor_pos_ != expressions_[cur_exp_].length()) {
			std::string text_to_skip(1, expressions_[cur_exp_][cursor_pos_++]);

			while (!isToken(text_to_skip)) {
				text_to_skip += expressions_[cur_exp_][cursor_pos_++];
			}

			if (text_to_skip == "i" and expressions_[cur_exp_][cursor_pos_] == 'n') {
				cursor_pos_ += 2;
			}

			if (text_to_skip == "tr" and expressions_[cur_exp_][cursor_pos_] == 'a') {
				cursor_pos_ += 3;
			}
		}
	}
}

void View::handleClearButton(float width, float height) {
	if (ImGui::Button("clr", ImVec2(width, height))) {
		if (init_matrix_ || init_system_ || is_answer_ready_ == systemAns) {
			return;
		}

		expressions_[cur_exp_] = "";
		cursor_pos_ = 0;
	}
}

void View::handleDeleteButton(float width, float height) {
	if (ImGui::Button("del", ImVec2(width, height)) && cursor_pos_) {
		if (is_answer_ready_ == systemAns) {
			return;
		}

		if (cursor_pos_) {
			std::string& text_to_use = getTextToUse();

			std::string text_to_del(1, text_to_use[--cursor_pos_]);

			while (!isToken(text_to_del)) {
				text_to_del = text_to_use[--cursor_pos_] + text_to_del;
			}

			std::string new_exp = "";
			for (int i = 0; i < cursor_pos_; ++i) {
				new_exp += text_to_use[i];
			}

			for (int i = cursor_pos_ + text_to_del.length(); i < text_to_use.length(); ++i) {
				new_exp += text_to_use[i];
			}

			text_to_use = new_exp;
		}
	}
}

void View::handleTypeButton(float width, float height) {
	if (ImGui::Button("type", ImVec2(width, height))) {
		if (init_matrix_ || init_system_ || is_answer_ready_) {
			return;
		}

		show_type_popup_ = true;
	}
}

void View::handleEqualButton(float width, float height) {
	if (ImGui::Button("=", ImVec2(width, height))) {
		if (is_answer_ready_ == systemAns) {
			is_answer_ready_ = noAns;
			return;
		}

		if (init_matrix_) {
			query_.type_ = type_;
			query_.type_of_query_ = init;
			query_.is_ans_used_ = is_ans_chosen_;
			query_.exp_ = "";
			query_.variable_used_ = cur_available_variable_;
			query_.matrix_ = variables_[cur_available_variable_];

			return;
		}

		if (init_system_) {
			query_.type_ = "";
			query_.type_of_query_ = solveEq;
			query_.is_ans_used_ = false;
			query_.exp_ = "";
			query_.matrix_ = equations_;

			return;
		}

		query_.type_ = type_;
		query_.type_of_query_ = calcExp;
		query_.is_ans_used_ = false;
		query_.exp_ = expressions_[cur_exp_];
	}
}

void View::handleEquationButton(float width, float height) {
	if (ImGui::Button("eq", ImVec2(width, height))) {
		if (init_matrix_ || init_system_ || is_answer_ready_) {
			return;
		}

		show_equation_popup_ = true;
	}
}

// checkers

// check if string is a token
bool View::isToken(const std::string& text) {
	for (int i = 0; i < configs_.BUTTONS_COUNT; ++i) {
		if (text == buttons_labels_[i]) {
			return true;
		}
	}

	return false;
}

// check if input modulo is valid
bool View::isModuloValid() {
	return !(type_.length() == 7 || type_[7] == '0');
}

// check if symbol can be pressed when initializing var or system
bool View::isEligibleForInitMatrix(const std::string& symbol) {
	if (symbol.length() != 1) {
		return false;
	}

	if (symbol[0] >= 48 && int(symbol[0]) <= 57) {
		return true;
	}

	if (symbol[0] == '.' || symbol[0] == '/' || symbol[0] == '-') {
		return true;
	}

	if ((symbol[0] == 'i' || symbol[0] == '+' || symbol[0] == '-') && type_ == "complex") {
		return true;
	}

	return false;
}

// check if system has no solution
bool View::isNoSolutionRow(const std::vector<float>& row) {
	bool all_zeros = true;
	for (int i = 0; i < row.size() - 1; ++i) {
		if (row[i] != 0.0) {
			all_zeros = false;
			break;
		}
	}

	if (all_zeros && row[row.size() - 1] != 0.0) {
		return true;
	}
	return false;
}

// getters
std::string& View::getTextToUse() {
	if (init_matrix_) {
		int row = cursor_cell_.first;
		int col = cursor_cell_.second;
		return variables_[cur_available_variable_][row][col];
	}

	if (init_system_) {
		int row = cursor_cell_.first;
		int col = cursor_cell_.second;
		return equations_[row][col];
	}

	return expressions_[cur_exp_];
}
