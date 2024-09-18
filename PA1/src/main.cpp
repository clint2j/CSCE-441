/**
* CSCE 441: Programming Assingment 1
* by Clint J Hart
*/

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <random>
#include <cstring>


#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

float frameBuffer[WINDOW_HEIGHT][WINDOW_WIDTH][3];
bool mask[WINDOW_HEIGHT][WINDOW_WIDTH];
GLFWwindow* window;


// Color structure. Can be used to define the brush and background color.
struct color { float r, g, b; };

color backgroundColor = { 0,0,0 };

color penColor = { 255,255,255 };

int cursorSize = 8;

bool squareCursor = true;

double xMousePos = 10;

double yMousePos = 10;

bool sprayPaint = false;

std::mt19937 mt{ std::random_device{}() };

std::uniform_int_distribution chance1_100{ 1, 100 };

// A function clamping the input values to the lower and higher bounds
#define CLAMP(in, low, high) ((in) < (low) ? (low) : ((in) > (high) ? (high) : in))

void switchCursor() {
	squareCursor = !squareCursor;
}

void enableSpray() {
	sprayPaint = !sprayPaint;
}

void SetFrameBufferPixel(int x, int y, struct color lc)
{
	y = WINDOW_HEIGHT - 1 - y;

	x = CLAMP(x, 0, WINDOW_WIDTH - 1);
	y = CLAMP(y, 0, WINDOW_HEIGHT - 1);

	frameBuffer[y][x][0] = lc.r;
	frameBuffer[y][x][1] = lc.g;
	frameBuffer[y][x][2] = lc.b;

}

void ChangeBackgroundColor(struct color c) {
	backgroundColor = c;
	for (int y = 0; y < WINDOW_HEIGHT; ++y) {
		for (int x = 0; x < WINDOW_WIDTH; ++x) {
			if (!mask[y][x]) {
				SetFrameBufferPixel(x, y, backgroundColor);
			}
		}
	}
}

void ClearDrawings() {
	for (int y = 0; y < WINDOW_HEIGHT; ++y) {
		for (int x = 0; x < WINDOW_WIDTH; ++x) {
			mask[(int)y][(int)x] = false;
			SetFrameBufferPixel(x, y, backgroundColor);
		}
	}
}

void ClearFrameBuffer()
{
	memset(frameBuffer, 0.0f, sizeof(float) * WINDOW_WIDTH * WINDOW_HEIGHT * 3);
}

void Display()
{
	glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_FLOAT, frameBuffer);
}

void colorPixel(double xpos, double ypos) {
	mask[CLAMP((int)ypos, 0, WINDOW_HEIGHT - 1)][CLAMP((int)xpos, 0, WINDOW_WIDTH - 1)] = true;
	SetFrameBufferPixel(xpos, ypos, penColor);
}

void draw(double xpos, double ypos) {
	if (sprayPaint) {
		int y_min = ypos - (cursorSize / 2), y_max = ypos + (cursorSize / 2);
		int x_min = xpos - (cursorSize / 2), x_max = xpos + (cursorSize / 2);
		int iXpos = xpos, iYpos = ypos;
		int rad_squared = (cursorSize / 2) * (cursorSize / 2);
		for (int x = x_min; x <= x_max; ++x) {
			int rootPart = sqrt(rad_squared - (x - iXpos) * (x - iXpos));
			int height = rootPart + iYpos;
			int l_height = -1 * rootPart + iYpos;
			int maxDistance = abs(xpos - x_min) + abs(ypos - y_min);
			for (int y = l_height; y <= height; ++y) {
				int roughDistance = abs(xpos - x) + abs(ypos - y);
				double percentOfMaxDis = roughDistance * 1.0 / maxDistance;
				int percentOfPixelsColored = (1 - percentOfMaxDis) * (1 - percentOfMaxDis) * 10;
				if (chance1_100(mt) <= percentOfPixelsColored)
					colorPixel(x, y);
			}
		}
	}
	else if (squareCursor) {
		for (int x = xpos - (cursorSize / 2); x <= xpos + (cursorSize / 2); ++x) {
			for (int y = ypos - (cursorSize / 2); y <= ypos + (cursorSize / 2); ++y) {
				colorPixel(x, y);
			}
		}
	}
	else {
		int y_min = ypos - (cursorSize / 2), y_max = ypos + (cursorSize / 2);
		int x_min = xpos - (cursorSize / 2), x_max = xpos + (cursorSize / 2);
		int iXpos = xpos, iYpos = ypos;
		int rad_squared = (cursorSize / 2) * (cursorSize / 2);
		for (int x = x_min; x <= x_max; ++x) {
			int rootPart = sqrt(rad_squared - (x - iXpos) * (x - iXpos));
			int height = rootPart + iYpos;
			int l_height = -1 * rootPart + iYpos;
			for (int y = l_height; y <= height; ++y) {
				colorPixel(x, y);
			}
		}
	}
}

// Mouse position callback function
void CursorPositionCallback(GLFWwindow* lWindow, double xpos, double ypos)
{
	xMousePos = xpos;
	yMousePos = ypos;
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if (state == GLFW_PRESS) {
		draw(xpos, ypos);
	}
}

// Mouse callback function
void MouseCallback(GLFWwindow* lWindow, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		ClearDrawings();
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		draw(xMousePos, yMousePos);
	}
}

void CharacterCallback(GLFWwindow* lWindow, unsigned int key)
{
	char letter = (char)key;
	std::cout << "Key " << letter << " is pressed." << std::endl;
	if (letter - '0' >= 0 && letter - '0' < 8) {
		int num = letter - '0';
		penColor = { 255.0f * (num & 0b100), 255.0f * (num & 0b10), 255.0f * (num & 1) };
		return;
	}
	switch (letter) {
	case '!':
		ChangeBackgroundColor({ 0,0,255 });
		break;
	case '@':
		ChangeBackgroundColor({ 0,255,0 });
		break;
	case '#':
		ChangeBackgroundColor({ 0,255,255 });
		break;
	case '$':
		ChangeBackgroundColor({ 255,0,0 });
		break;
	case '%':
		ChangeBackgroundColor({ 255,0,255 });
		break;
	case '^':
		ChangeBackgroundColor({ 255,255,0 });
		break;
	case '&':
		ChangeBackgroundColor({ 255,255,255 });
		break;
	case '-':
		cursorSize = CLAMP(cursorSize / 2, 1, 128);
		break;
	case '+':
		cursorSize = CLAMP(cursorSize * 2, 1, 128);
		break;
	case 'b':
		switchCursor();
		break;
	case 's':
		enableSpray();
		break;
	}

}

void Init()
{
	glfwInit();
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GL_FALSE);
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Assignment 1 - Clint Hart", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetMouseButtonCallback(window, MouseCallback);
	glfwSetCursorPosCallback(window, CursorPositionCallback);
	glfwSetCharCallback(window, CharacterCallback);
	glewExperimental = GL_TRUE;
	glewInit();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	ClearFrameBuffer();
}


int main()
{
	Init();
	while (glfwWindowShouldClose(window) == 0)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		Display();
		glFlush();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}