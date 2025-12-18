#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <gl2d/gl2d.h>
#include <engine/debug/openglErrorReporting.h>
#include <engine/Shader.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imguiThemes.h"

#include <game/SnakeGame.h>

#pragma region CrowFramework_Config
/// ============================================================================
/// CrowFramework Configuration
/// ----------------------------------------------------------------------------
/// - Global engine configuration values.
/// - Window size, title, debug flags, etc.
/// - Safe to edit.
/// ============================================================================
static constexpr int kDefaultWidth = 640;
static constexpr int kDefaultHeight = 480;
static constexpr const char* kWindowTitle = "CrowFramework Sandbox";
#pragma endregion

#pragma region Platform_Callbacks
/// ============================================================================
/// Platform Callbacks
/// ----------------------------------------------------------------------------
/// - Low-level platform callbacks (GLFW).
/// - You usually DO NOT need to modify this.
/// ============================================================================
static void error_callback(int error, const char* description)
{
	std::cout << "GLFW Error(" << error << "): " << description << "\n";
}
#pragma endregion

#pragma region Game_TemporaryData
/// ============================================================================
/// Temporary Game Data
/// ----------------------------------------------------------------------------
/// - Quick test data used during prototyping.
/// - Replace with Mesh / Resource system later.
/// ============================================================================
static float rect[] = {
	-0.5f,-0.5f,0.0f,
	 0.5f,-0.5f,0.0f,
	 0.5f, 0.5f,0.0f,

	-0.5f,-0.5f,0.0f,
	 0.5f, 0.5f,0.0f,
	-0.5f, 0.5f,0.0f
};
#pragma endregion

#pragma region Main
int main()
{
#pragma region Engine_Startup
	/// ========================================================================
	/// Engine Startup
	/// ------------------------------------------------------------------------
	/// - Initializes the platform layer.
	/// - Creates the window and OpenGL context.
	/// - Do NOT put game logic here.
	/// ========================================================================
	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window =
		glfwCreateWindow(kDefaultWidth, kDefaultHeight, kWindowTitle, nullptr, nullptr);

	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
#pragma endregion

#pragma region Graphics_Startup
	/// ========================================================================
	/// Graphics Startup
	/// ------------------------------------------------------------------------
	/// - Loads OpenGL function pointers (GLAD).
	/// - Sets up debug and error reporting.
	/// ========================================================================
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

	enableReportGlErrors();
	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
#pragma endregion

#pragma region Resource_Loading
	/// ========================================================================
	/// Resource Loading
	/// ------------------------------------------------------------------------
	/// - Load shaders, meshes, textures, and other GPU resources here.
	/// - This section will later be replaced by a ResourceManager.
	/// ========================================================================

	GLuint vao = 0, vbo = 0;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	Shader shader("assets/shaders/basic.vert", "assets/shaders/basic.frag");
	if (!shader.IsValid())
	{
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}
#pragma endregion

#pragma region Game_Initialization
	/// ========================================================================
	/// Game Initialization
	/// ------------------------------------------------------------------------
	/// - Initialize game state here.
	/// - Player, enemies, levels, scores, etc.
	/// ========================================================================

	SnakeGame snake(32, 18);

	// ---- ImGui init (once) ----
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark(); // or your theme

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

#pragma endregion

#pragma region Main_Loop
	/// ========================================================================
	/// Main Game Loop
	/// ------------------------------------------------------------------------
	/// - Runs until the window is closed.
	/// - Update ¡æ Render ¡æ Present
	/// ========================================================================


	while (!glfwWindowShouldClose(window))
	{
#pragma region Frame_Begin
		/// Per-frame setup (viewport, clear, timing).
		int width = 0, height = 0;
		glfwGetFramebufferSize(window, &width, &height);

		static double lastTime = glfwGetTime();
		double now = glfwGetTime();
		float dt = float(now - lastTime);
		lastTime = now;

		glfwPollEvents();

		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);
#pragma endregion

#pragma region Input_Update
		/// --------------------------------------------------------------------
		/// Input Update
		/// - Handle keyboard / mouse input here.
		/// --------------------------------------------------------------------
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
			snake.SetPendingDir(Dir::Up);
		else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
			snake.SetPendingDir(Dir::Down);
		else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			snake.SetPendingDir(Dir::Left);
		else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			snake.SetPendingDir(Dir::Right);

		static bool prevR = false;
		bool currR = glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS;

		if (currR && !prevR)
		{
			snake.Reset();
		}

		prevR = currR;

		snake.Update(dt);

#pragma endregion

#pragma region Game_Update
		/// --------------------------------------------------------------------
		/// Game Update
		/// - Update game logic.
		/// - Movement, collision, AI, scoring, etc.
		/// --------------------------------------------------------------------
#pragma endregion

#pragma region World_Render
		/// --------------------------------------------------------------------
		/// World Rendering
		/// - Draw game objects here.
		/// - Do NOT update game logic in this section.
		/// --------------------------------------------------------------------
		shader.Use();

		const int gw = snake.GetGridW();
		const int gh = snake.GetGridH();

		// cell size in NDC ([-1, +1])
		const float cellW = 2.0f / float(gw);
		const float cellH = 2.0f / float(gh);

		// our rect is centered at (0,0) with size 1 (from -0.5 to +0.5),
		// so scaling by (cellW, cellH) makes it exactly one grid cell.
		shader.SetVec2("uScale", cellW, cellH);

		// grid cell -> NDC center (top-left origin, y goes down)
		auto cellToNDC = [&](int x, int y)
			{
				float nx = -1.0f + cellW * (float(x) + 0.5f);
				float ny = 1.0f - cellH * (float(y) + 0.5f);
				return std::pair<float, float>(nx, ny);
			};

		glBindVertexArray(vao);

		// --- draw food (red) ---
		{
			const Cell& f = snake.GetFood();
			auto [fx, fy] = cellToNDC(f.x, f.y);
			shader.SetVec3("uColor", 1.0f, 0.0f, 0.0f);
			shader.SetVec2("uOffset", fx, fy);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		// --- draw body (green) ---
		{
			shader.SetVec3("uColor", 0.0f, 1.0f, 0.0f);
			const auto& body = snake.GetBody();

			// draw every segment
			for (const Cell& c : body)
			{
				auto [ox, oy] = cellToNDC(c.x, c.y);
				shader.SetVec2("uOffset", ox, oy);
				glDrawArrays(GL_TRIANGLES, 0, 6);
			}
		}

		// --- draw head (brighter green) ---
		{
			const Cell& h = snake.GetHead();
			auto [hx, hy] = cellToNDC(h.x, h.y);
			shader.SetVec3("uColor", 0.2f, 1.0f, 0.2f);
			shader.SetVec2("uOffset", hx, hy);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		glBindVertexArray(0);

#pragma endregion

#pragma region UI_Render
		/// --------------------------------------------------------------------
		/// UI Rendering
		/// - ImGui or HUD rendering goes here.
		/// --------------------------------------------------------------------
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::SetNextWindowSize(ImVec2(260, 140), ImGuiCond_Always);
		ImGui::Begin("Snake");

		ImGui::Text("Score (Length): %d", snake.GetScore());
		ImGui::Text("Length: %d", (int)snake.GetBody().size());

		if (snake.IsGameOver())
		{
			ImGui::Separator();
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "GAME OVER");
			ImGui::Text("Press R to restart");
		}

		ImGui::End();


		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

#pragma endregion

#pragma region Frame_End
		/// Present the frame and poll events.
		glfwSwapBuffers(window);
#pragma endregion
	}
#pragma endregion

#pragma region Engine_Shutdown
	/// ========================================================================
	/// Engine Shutdown
	/// ------------------------------------------------------------------------
	/// - Clean up all resources.
	/// - Called once before application exit.
	/// ========================================================================
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
#pragma endregion
}
#pragma endregion