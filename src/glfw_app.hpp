#pragma once

#include <Ignis/Ignis.h>
#include <GLFW/glfw3.h>

#include "mathutils/ArcBall.h"
#include "mathutils/CPosition.h"
#include "mathutils/CVector.h"

#define DEG2RAD(a) ((a)/57.295)

class GLFWApplication
{
protected:
    GLFWwindow* window;

	int m_nWidth, m_nHeight;
	double m_dFieldOfView;
	double m_dCenter, m_dSceneRadius, m_dZoom;

	CArcBall* m_pCameraArcball;
	CArcBall* m_pLightArcball;

	CPosition m_vecMouseDown;
	bool m_bDragging;
	bool m_bZooming;
	bool m_bRotating;
	bool m_bExternalMouseHandling;

	CVector m_vecCameraPosition;
	double m_dTranslateX, m_dTranslateY;

	int m_nMouseState;
	int m_nInteractionMode;
public:
	enum
	{
		MBUTTON_LEFT = 0x01,
		MBUTTON_MIDDLE = 0x02,
		MBUTTON_RIGHT = 0x04,
		KBUTTON_CTRL = 0x08,
		KBUTTON_SHIFT = 0x10
	};

    GLFWApplication();
    ~GLFWApplication();

    void run();

	virtual void onWindowResize(int width, int height);
	virtual void onKeyEvent(int key, int action);
	virtual void onMouseButtonEvent(int button, int state, int x, int y);
	virtual void onMouseMoveEvent(int x, int y);

    virtual void render() = 0;
};