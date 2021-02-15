#include "System/Typedefs.h"
#include "Rendering/VertexBuffer.h"
#include "Rendering/IndexBuffer.h"
#include "Rendering/Model.h"

#include <windows.h>
#include <iostream>
#include <stdio.h>

bool running = true;
HINSTANCE hInstance;

template< Char Letter >
struct VirtualKey {
	enum {
		Code = (Letter - 'A') + 0x41
	};
};

// The window callback function
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch (message) {
		case WM_SIZE:
			{
				
				break;
			}

		case WM_KEYDOWN:
			{   
				
				break;
			} 

		case WM_DESTROY:
			{
				PostQuitMessage(0);
				break;
			}
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

// Main entry point
int WINAPI WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR    lpCmdLine,
                     int       nCmdShow) 
{
	MSG msg;
	WNDCLASS windowClass;
	HWND hWnd;
	DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	const int width = 1024;
	const int height = 768;

	hInstance = GetModuleHandle(NULL);

	windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = (WNDPROC) WndProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = hInstance;
	windowClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = NULL;
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = TEXT("DeferredRenderingClass");

	if (!RegisterClass(&windowClass)) {
		return false;
	}

	hWnd = CreateWindowEx(dwExStyle, TEXT("DeferredRenderingClass"),  TEXT("Mesh Editor"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, width, height, NULL, NULL, hInstance, NULL);

	//if(!application.initialize(hWnd, width, height))
	//	return 0;

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	VertexBuffer	vertexBuffer;
	IndexBuffer		indexBuffer;

/*
glTexCoord2f(0.0f, 0.0f); glNormal3f(0.0f, 0.0f,  1.0f); glVertex3f(-1.0f, -1.0f,  1.0f); 
glTexCoord2f(1.0f, 0.0f); glNormal3f(0.0f, 0.0f,  1.0f); glVertex3f( 1.0f, -1.0f,  1.0f); 
glTexCoord2f(1.0f, 1.0f); glNormal3f(0.0f, 0.0f,  1.0f); glVertex3f( 1.0f,  1.0f,  1.0f); 
glTexCoord2f(0.0f, 1.0f); glNormal3f(0.0f, 0.0f,  1.0f); glVertex3f(-1.0f,  1.0f,  1.0f); 
// Back Face
glTexCoord2f(1.0f, 0.0f); glNormal3f(0.0f, 0.0f, -1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
glTexCoord2f(1.0f, 1.0f); glNormal3f(0.0f, 0.0f, -1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
glTexCoord2f(0.0f, 1.0f); glNormal3f(0.0f, 0.0f, -1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
glTexCoord2f(0.0f, 0.0f); glNormal3f(0.0f, 0.0f, -1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
// Top Face
glTexCoord2f(0.0f, 1.0f); glNormal3f(0.0f, 1.0f,  0.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
glTexCoord2f(0.0f, 0.0f); glNormal3f(0.0f, 1.0f,  0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
glTexCoord2f(1.0f, 0.0f); glNormal3f(0.0f, 1.0f,  0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
glTexCoord2f(1.0f, 1.0f); glNormal3f(0.0f, 1.0f,  0.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
// Bottom Face
glTexCoord2f(1.0f, 1.0f); glNormal3f(0.0f, -1.0f,  0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
glTexCoord2f(0.0f, 1.0f); glNormal3f(0.0f, -1.0f,  0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
glTexCoord2f(0.0f, 0.0f); glNormal3f(0.0f, -1.0f,  0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
glTexCoord2f(1.0f, 0.0f); glNormal3f(0.0f, -1.0f,  0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
// Right face
glTexCoord2f(1.0f, 0.0f); glNormal3f(1.0f, 0.0f,  0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
glTexCoord2f(1.0f, 1.0f); glNormal3f(1.0f, 0.0f,  0.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
glTexCoord2f(0.0f, 1.0f); glNormal3f(1.0f, 0.0f,  0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
glTexCoord2f(0.0f, 0.0f); glNormal3f(1.0f, 0.0f,  0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
// Left Face
glTexCoord2f(0.0f, 0.0f); glNormal3f(-1.0f, 0.0f,  0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
glTexCoord2f(1.0f, 0.0f); glNormal3f(-1.0f, 0.0f,  0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
glTexCoord2f(1.0f, 1.0f); glNormal3f(-1.0f, 0.0f,  0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
glTexCoord2f(0.0f, 1.0f); glNormal3f(-1.0f, 0.0f,  0.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
*/

	// Front Face
	//glTexCoord2f(0.0f, 0.0f); glNormal3f(0.0f, 0.0f,  1.0f); glVertex3f(-1.0f, -1.0f,  1.0f); 
	//glTexCoord2f(1.0f, 0.0f); glNormal3f(0.0f, 0.0f,  1.0f); glVertex3f( 1.0f, -1.0f,  1.0f); 
	//glTexCoord2f(1.0f, 1.0f); glNormal3f(0.0f, 0.0f,  1.0f); glVertex3f( 1.0f,  1.0f,  1.0f); 
	//glTexCoord2f(0.0f, 1.0f); glNormal3f(0.0f, 0.0f,  1.0f); glVertex3f(-1.0f,  1.0f,  1.0f); 
	vertexBuffer.AddTextureCoordinate( VertexBuffer::TTextureCoordinate(0.0f, 0.0f) );
	vertexBuffer.AddNormal( VertexBuffer::TNormal(0.0f, 0.0f, 1.0f) );
	vertexBuffer.AddPosition( VertexBuffer::TPosition(-1.0f, -1.0f,  1.0f) );

	vertexBuffer.AddTextureCoordinate( VertexBuffer::TTextureCoordinate(1.0f, 0.0f) );
	vertexBuffer.AddNormal( VertexBuffer::TNormal(0.0f, 0.0f, 1.0f) );
	vertexBuffer.AddPosition( VertexBuffer::TPosition(-1.0f, -1.0f,  1.0f) );

	vertexBuffer.AddTextureCoordinate( VertexBuffer::TTextureCoordinate(1.0f, 1.0f) );
	vertexBuffer.AddNormal( VertexBuffer::TNormal(0.0f, 0.0f, 1.0f) );
	vertexBuffer.AddPosition( VertexBuffer::TPosition(1.0f, 1.0f,  1.0f) );

	vertexBuffer.AddTextureCoordinate( VertexBuffer::TTextureCoordinate(0.0f, 1.0f) );
	vertexBuffer.AddNormal( VertexBuffer::TNormal(0.0f, 0.0f, 1.0f) );
	vertexBuffer.AddPosition( VertexBuffer::TPosition(-1.0f, 1.0f,  1.0f) );

	// Back Face
	//glTexCoord2f(1.0f, 0.0f); glNormal3f(0.0f, 0.0f, -1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	//glTexCoord2f(1.0f, 1.0f); glNormal3f(0.0f, 0.0f, -1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
	//glTexCoord2f(0.0f, 1.0f); glNormal3f(0.0f, 0.0f, -1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
	//glTexCoord2f(0.0f, 0.0f); glNormal3f(0.0f, 0.0f, -1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
	vertexBuffer.AddTextureCoordinate( VertexBuffer::TTextureCoordinate(1.0f, 0.0f) );
	vertexBuffer.AddNormal( VertexBuffer::TNormal(0.0f, 0.0f, -1.0f) );
	vertexBuffer.AddPosition( VertexBuffer::TPosition(-1.0f, -1.0f, -1.0f) );

	vertexBuffer.AddTextureCoordinate( VertexBuffer::TTextureCoordinate(1.0f, 1.0f) );
	vertexBuffer.AddNormal( VertexBuffer::TNormal(0.0f, 0.0f, -1.0f) );
	vertexBuffer.AddPosition( VertexBuffer::TPosition(-1.0f, 1.0f, -1.0f) );

	vertexBuffer.AddTextureCoordinate( VertexBuffer::TTextureCoordinate(0.0f, 1.0f) );
	vertexBuffer.AddNormal( VertexBuffer::TNormal(0.0f, 0.0f, -1.0f) );
	vertexBuffer.AddPosition( VertexBuffer::TPosition(1.0f, 1.0f, -1.0f) );

	vertexBuffer.AddTextureCoordinate( VertexBuffer::TTextureCoordinate(0.0f, 0.0f) );
	vertexBuffer.AddNormal( VertexBuffer::TNormal(0.0f, 0.0f, -1.0f) );
	vertexBuffer.AddPosition( VertexBuffer::TPosition(1.0f, -1.0f, -1.0f) );

	// Top Face
	//glTexCoord2f(0.0f, 1.0f); glNormal3f(0.0f, 1.0f,  0.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
	//glTexCoord2f(0.0f, 0.0f); glNormal3f(0.0f, 1.0f,  0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
	//glTexCoord2f(1.0f, 0.0f); glNormal3f(0.0f, 1.0f,  0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
	//glTexCoord2f(1.0f, 1.0f); glNormal3f(0.0f, 1.0f,  0.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
	vertexBuffer.AddTextureCoordinate( VertexBuffer::TTextureCoordinate(0.0f, 1.0f) );
	vertexBuffer.AddNormal( VertexBuffer::TNormal(0.0f, 1.0f, 0.0f) );
	vertexBuffer.AddPosition( VertexBuffer::TPosition(-1.0f, 1.0f, -1.0f) );

	vertexBuffer.AddTextureCoordinate( VertexBuffer::TTextureCoordinate(0.0f, 0.0f) );
	vertexBuffer.AddNormal( VertexBuffer::TNormal(0.0f, 1.0f, 0.0f) );
	vertexBuffer.AddPosition( VertexBuffer::TPosition(-1.0f, 1.0f, 1.0f) );

	vertexBuffer.AddTextureCoordinate( VertexBuffer::TTextureCoordinate(1.0f, 0.0f) );
	vertexBuffer.AddNormal( VertexBuffer::TNormal(0.0f, 1.0f, 0.0f) );
	vertexBuffer.AddPosition( VertexBuffer::TPosition(1.0f, 1.0f, 1.0f) );

	vertexBuffer.AddTextureCoordinate( VertexBuffer::TTextureCoordinate(1.0f, 1.0f) );
	vertexBuffer.AddNormal( VertexBuffer::TNormal(0.0f, 1.0f, 0.0f) );
	vertexBuffer.AddPosition( VertexBuffer::TPosition(1.0f, 1.0f, -1.0f) );

	// Bottom Face
	//glTexCoord2f(1.0f, 1.0f); glNormal3f(0.0f, -1.0f,  0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	//glTexCoord2f(0.0f, 1.0f); glNormal3f(0.0f, -1.0f,  0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
	//glTexCoord2f(0.0f, 0.0f); glNormal3f(0.0f, -1.0f,  0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
	//glTexCoord2f(1.0f, 0.0f); glNormal3f(0.0f, -1.0f,  0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
	vertexBuffer.AddTextureCoordinate( VertexBuffer::TTextureCoordinate(1.0f, 1.0f) );
	vertexBuffer.AddNormal( VertexBuffer::TNormal(0.0f, -1.0f, 0.0f) );
	vertexBuffer.AddPosition( VertexBuffer::TPosition(-1.0f, -1.0f, -1.0f) );

	vertexBuffer.AddTextureCoordinate( VertexBuffer::TTextureCoordinate(0.0f, 1.0f) );
	vertexBuffer.AddNormal( VertexBuffer::TNormal(0.0f, -1.0f, 0.0f) );
	vertexBuffer.AddPosition( VertexBuffer::TPosition(1.0f, -1.0f, -1.0f) );

	vertexBuffer.AddTextureCoordinate( VertexBuffer::TTextureCoordinate(0.0f, 0.0f) );
	vertexBuffer.AddNormal( VertexBuffer::TNormal(0.0f, -1.0f, 0.0f) );
	vertexBuffer.AddPosition( VertexBuffer::TPosition(1.0f, -1.0f, 1.0f) );

	vertexBuffer.AddTextureCoordinate( VertexBuffer::TTextureCoordinate(1.0f, 0.0f) );
	vertexBuffer.AddNormal( VertexBuffer::TNormal(0.0f, -1.0f, 0.0f) );
	vertexBuffer.AddPosition( VertexBuffer::TPosition(-1.0f, -1.0f, 1.0f) );

	// Right face
	//glTexCoord2f(1.0f, 0.0f); glNormal3f(1.0f, 0.0f,  0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
	//glTexCoord2f(1.0f, 1.0f); glNormal3f(1.0f, 0.0f,  0.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
	//glTexCoord2f(0.0f, 1.0f); glNormal3f(1.0f, 0.0f,  0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
	//glTexCoord2f(0.0f, 0.0f); glNormal3f(1.0f, 0.0f,  0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
	vertexBuffer.AddTextureCoordinate( VertexBuffer::TTextureCoordinate(1.0f, 0.0f) );
	vertexBuffer.AddNormal( VertexBuffer::TNormal(1.0f, 0.0f, 0.0f) );
	vertexBuffer.AddPosition( VertexBuffer::TPosition(1.0f, -1.0f, -1.0f) );

	vertexBuffer.AddTextureCoordinate( VertexBuffer::TTextureCoordinate(1.0f, 1.0f) );
	vertexBuffer.AddNormal( VertexBuffer::TNormal(1.0f, 0.0f, 0.0f) );
	vertexBuffer.AddPosition( VertexBuffer::TPosition(1.0f, 1.0f, -1.0f) );

	vertexBuffer.AddTextureCoordinate( VertexBuffer::TTextureCoordinate(0.0f, 1.0f) );
	vertexBuffer.AddNormal( VertexBuffer::TNormal(1.0f, 0.0f, 0.0f) );
	vertexBuffer.AddPosition( VertexBuffer::TPosition(1.0f, 1.0f, 1.0f) );

	vertexBuffer.AddTextureCoordinate( VertexBuffer::TTextureCoordinate(0.0f, 0.0f) );
	vertexBuffer.AddNormal( VertexBuffer::TNormal(1.0f, 0.0f, 0.0f) );
	vertexBuffer.AddPosition( VertexBuffer::TPosition(1.0f, -1.0f, 1.0f) );

	// Left Face
	//glTexCoord2f(0.0f, 0.0f); glNormal3f(-1.0f, 0.0f,  0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	//glTexCoord2f(1.0f, 0.0f); glNormal3f(-1.0f, 0.0f,  0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
	//glTexCoord2f(1.0f, 1.0f); glNormal3f(-1.0f, 0.0f,  0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
	//glTexCoord2f(0.0f, 1.0f); glNormal3f(-1.0f, 0.0f,  0.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
	vertexBuffer.AddTextureCoordinate( VertexBuffer::TTextureCoordinate(0.0f, 0.0f) );
	vertexBuffer.AddNormal( VertexBuffer::TNormal(-1.0f, 0.0f, 0.0f) );
	vertexBuffer.AddPosition( VertexBuffer::TPosition(-1.0f, -1.0f, -1.0f) );

	vertexBuffer.AddTextureCoordinate( VertexBuffer::TTextureCoordinate(1.0f, 0.0f) );
	vertexBuffer.AddNormal( VertexBuffer::TNormal(-1.0f, 0.0f, 0.0f) );
	vertexBuffer.AddPosition( VertexBuffer::TPosition(-1.0f, -1.0f, 1.0f) );

	vertexBuffer.AddTextureCoordinate( VertexBuffer::TTextureCoordinate(1.0f, 1.0f) );
	vertexBuffer.AddNormal( VertexBuffer::TNormal(-1.0f, 0.0f, 0.0f) );
	vertexBuffer.AddPosition( VertexBuffer::TPosition(-1.0f, 1.0f, 1.0f) );

	vertexBuffer.AddTextureCoordinate( VertexBuffer::TTextureCoordinate(0.0f, 1.0f) );
	vertexBuffer.AddNormal( VertexBuffer::TNormal(-1.0f, 0.0f, 0.0f) );
	vertexBuffer.AddPosition( VertexBuffer::TPosition(-1.0f, 1.0f, -1.0f) );

#define ADD_FACE_INDICES( index ) \
indexBuffer.AddIndex( 0 + (index * 4) );	\
indexBuffer.AddIndex( 1 + (index * 4) );	\
indexBuffer.AddIndex( 2 + (index * 4) );	\
indexBuffer.AddIndex( 2 + (index * 4) );	\
indexBuffer.AddIndex( 3 + (index * 4) );	\
indexBuffer.AddIndex( 0 + (index * 4) );

	ADD_FACE_INDICES( 0 );
	ADD_FACE_INDICES( 1 );
	ADD_FACE_INDICES( 2 );
	ADD_FACE_INDICES( 3 );
	ADD_FACE_INDICES( 4 );
	ADD_FACE_INDICES( 5 );

	File file;

	file.Open( "Cube.mesh", "wb" );

	Model model( vertexBuffer, indexBuffer );
	model.WriteTo( file );

	file.Close();

	file.Open("Cube.mesh", "rb");
	Model testModel;
	
	verify( testModel.ReadFrom(file) );

	bool running = true;
	while (running)
	{
		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
		{
			if (msg.message == WM_QUIT)
			{
				running = false;
				break;
			}
			else 
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		
		//application.update();
		//application.render();
	}

	//application.release();

	return (int) msg.wParam;
}