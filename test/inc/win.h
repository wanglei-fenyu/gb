#include "font.h"
#include "item.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <vector>
#include <windows.h>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif
//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" ) 
static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}



class Win
{
public:
	static Win* GetMainWin()
	{
		if (instance_ == nullptr)
			instance_ = new Win();
		return instance_;
	}

	void Run();
	void Stop();
public:
	//Item *cell;
	std::vector<Item> items;

private:
	Win();
	void Init();
	void Destory();
	void TickHead();
	void TickTail();

private:
	bool is_open;
	ImGuiIO* io_ptr;
	GLFWwindow* window;
	inline static Win* instance_;

	int tick_hz;
};
