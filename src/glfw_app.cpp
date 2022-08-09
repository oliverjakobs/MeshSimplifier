#include "glfw_app.hpp"

#include <math.h>
#include <stdio.h>

#define ZOOM_MIN (0.05)
#define ZOOM_MAX (10.0)

static void glfwErrorCallback(int error, const char* desc);
static void ignisErrorCallback(ignisErrorLevel level, const char* desc);

static void GLFWWindowSizeCallback(GLFWwindow* window, int width, int height);
static void GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
static void GLFWCursorPosCallback(GLFWwindow* window, double x, double y);

GLFWApplication::GLFWApplication()
{
	// Setup window
	glfwSetErrorCallback(glfwErrorCallback);
	if (!glfwInit())
		return;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	// Create window with graphics context
	window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
	if (window == NULL)
		return;

	glfwMakeContextCurrent(window);
	glfwSetWindowUserPointer(window, this);
	glfwSwapInterval(1); // Enable vsync

	m_pCameraArcball = new CArcBall();
	m_pLightArcball = new CArcBall();
	m_dFieldOfView = 30.0;
	m_nWidth = 1;
	m_nHeight = 1;

	m_dSceneRadius = 15.0;
	m_vecCameraPosition.set(0.0, 0.0, 0.0, 1.0);

	m_dCenter = m_dSceneRadius / sin(DEG2RAD(m_dFieldOfView) / 2.0);

	m_dZoom = 1.0;
	m_dTranslateX = 0.0;
	m_dTranslateY = 0.0;
	m_nMouseState = 0;

	m_bDragging = false;
	m_bZooming = false;
	m_bRotating = false;
	m_bExternalMouseHandling = false;

	// set glfw callbacks
	glfwSetWindowSizeCallback(window, GLFWWindowSizeCallback);
	glfwSetKeyCallback(window, GLFWKeyCallback);
	glfwSetMouseButtonCallback(window, GLFWMouseButtonCallback);
	glfwSetCursorPosCallback(window, GLFWCursorPosCallback);

	ignisSetErrorCallback(ignisErrorCallback);
	if (!ignisInit(true))
		return;

	ignisSetClearColor(IGNIS_DARK_GREY);
}

GLFWApplication::~GLFWApplication()
{
	if (NULL != m_pCameraArcball) delete m_pCameraArcball;
	if (NULL != m_pLightArcball)  delete m_pLightArcball;

	glfwDestroyWindow(window);
	glfwTerminate();
}

void GLFWApplication::run()
{
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

		// The usual OpenGL stuff to clear the screen and set up viewing.
		glClearColor(.5, .5, 1., 1.);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        render();

        glfwSwapBuffers(window);
    }
}

void GLFWApplication::onWindowResize(int width, int height)
{
	m_nWidth = width;
	m_nHeight = height;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	m_pCameraArcball->place(CPosition(width / 2, height / 2), 0.5 * sqrt(double(width * width + height * height)));
}

void GLFWApplication::onKeyEvent(int key, int action)
{

}

void GLFWApplication::onMouseButtonEvent(int button, int action, int x, int y)
{
	switch (action) {
	case GLFW_PRESS:
		m_vecMouseDown.set(double(x) / double(m_nWidth), double(y) / double(m_nHeight));

		switch (button) {
		case GLFW_MOUSE_BUTTON_LEFT:
			m_nMouseState |= (MBUTTON_LEFT);
			m_pCameraArcball->setMouse(CPosition(x, m_nHeight - 1 - y));
			m_pCameraArcball->startDragging();
			m_bRotating = true;
			break;
		case GLFW_MOUSE_BUTTON_RIGHT:
			m_nMouseState |= (MBUTTON_RIGHT);
			m_bDragging = false;
			m_bZooming = true;
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			m_nMouseState |= (MBUTTON_MIDDLE);
			m_bDragging = true;
			m_bZooming = false;
			break;
		default:
			break;
		}
		break;
	case GLFW_RELEASE:
		switch (button) {
		case GLFW_MOUSE_BUTTON_LEFT:
			m_nMouseState ^= MBUTTON_LEFT;
			m_pCameraArcball->finishDragging();
			break;
		case GLFW_MOUSE_BUTTON_RIGHT:
			m_nMouseState ^= MBUTTON_RIGHT;
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			m_nMouseState ^= MBUTTON_MIDDLE;
			break;
		default:
			break;
		}
		m_bRotating = false;
		m_bDragging = false;
		m_bZooming = false;
		m_bExternalMouseHandling = false;
	}
}

void GLFWApplication::onMouseMoveEvent(int x, int y)
{
	CPosition pos(double(x) / double(m_nWidth), double(y) / double(m_nHeight));

	CPosition drag = pos - m_vecMouseDown;

	if ((m_bRotating) && (m_nMouseState & (MBUTTON_LEFT)))
	{
		m_pCameraArcball->setMouse(CPosition(x, m_nHeight - 1 - y));
	}
	else if ((m_bDragging) && (m_nMouseState & MBUTTON_MIDDLE))
	{
		m_dTranslateX += 2.0 * drag[0];
		m_dTranslateY += -2.0 * drag[1];

		if (m_dTranslateX > m_dZoom) m_dTranslateX = m_dZoom;
		if (m_dTranslateX < -m_dZoom) m_dTranslateX = -m_dZoom;
		if (m_dTranslateY > m_dZoom) m_dTranslateY = m_dZoom;
		if (m_dTranslateY < -m_dZoom) m_dTranslateY = -m_dZoom;
	}
	else if ((m_bZooming) && (m_nMouseState & MBUTTON_RIGHT))
	{
		double dZoom = 10.0 * drag[0];
		m_dZoom += dZoom;
		if (m_dZoom < ZOOM_MIN) m_dZoom = ZOOM_MIN;
		if (m_dZoom > ZOOM_MAX) m_dZoom = ZOOM_MAX;
		if (m_dTranslateX > m_dZoom) m_dTranslateX = m_dZoom;
		if (m_dTranslateX < -m_dZoom) m_dTranslateX = -m_dZoom;
		if (m_dTranslateY > m_dZoom) m_dTranslateY = m_dZoom;
		if (m_dTranslateY < -m_dZoom) m_dTranslateY = -m_dZoom;
	}
	m_vecMouseDown = pos;
}

void glfwErrorCallback(int error, const char* desc)
{
	printf("[Glfw] %d: %s\n", error, desc);
}

void ignisErrorCallback(ignisErrorLevel level, const char* desc)
{
	switch (level)
	{
	case IGNIS_WARN:     printf("%s\n", desc); break;
	case IGNIS_ERROR:    printf("%s\n", desc); break;
	case IGNIS_CRITICAL: printf("%s\n", desc); break;
	}
}

void GLFWWindowSizeCallback(GLFWwindow* window, int width, int height)
{
	auto app = (GLFWApplication*)glfwGetWindowUserPointer(window);
	if (app) app->onWindowResize(width, height);
}

void GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	auto app = (GLFWApplication*)glfwGetWindowUserPointer(window);
	if (app) app->onKeyEvent(key, action);
}

void GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	auto app = (GLFWApplication*)glfwGetWindowUserPointer(window);
	if (app)
	{
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		app->onMouseButtonEvent(button, action, (int)x, (int)y);
	}
}

void GLFWCursorPosCallback(GLFWwindow* window, double x, double y)
{
	auto app = (GLFWApplication*)glfwGetWindowUserPointer(window);
	if (app) app->onMouseMoveEvent((int)x, (int)y);
}