#include "cgg/Graphics.h"
#include "cgg/BitmapImage.h"
#include "mdl/MeshFile.h"
#include "mdl/Texture.h"
#include "mdl/Scene.h"
#include <iostream>
#include <Windows.h>
#include <GL/gl.h>
#include <math.h>

std::vector<VertexNormalColour> g_mapPoints;//the array for height map
std::vector<int> g_indices; //the array of indices to draw the height map


/*******************************
***	C **************************
***	  |\ ***********************
***	^ | \ **********************
***	| |  \ *********************
***	  ----- ********************
***	A -----> B *****************
********************************/

Vec3 getNormal(Vec3 a, Vec3 b, Vec3 c) //a function to work out the normals for the map
{
	Vec3 ab = b - a; //up
	Vec3 ac = c - a; //right

	Vec3 normals;
	normals.x = (ac.y*ab.z) - (ac.z*ab.y);//work out the x of the normal
	normals.y = -((ab.z*ac.x) - (ab.x*ac.z));//work out the y of the normal
	normals.z = (ac.x*ab.y) - (ac.y*ab.x);//work out the z of the normal

	//the factor that I need to divide by to work out the normal
	float factor = sqrt((normals.x*normals.x) + (normals.y*normals.y) + (normals.z*normals.z));

	//works out the normals
	normals.x = normals.x / factor;
	normals.y = normals.y / factor;
	normals.z = normals.z / factor;
	
	return normals;// returns the normal
}

//------------------------------------------------------------------------------------------------------------------------------------
// called when we initialise the app
//------------------------------------------------------------------------------------------------------------------------------------
void init()
{
	// give our app a title 
	setWindowTitle("CGG Assignment 2 - Heightmap: Jamie Slowgrove");

	// sets the size of the 2D screen coordinates. I'm just initialising the defaults here. You can use whatever you want!
	// args are: minx, maxx, miny, maxy
	setScreenCoordinates(-20.0f, 20.0f, -15.0f, 15.0f);

	// setting the background colour to a default grey!
	setBackgroundColour(0.1f, 0.1f, 0.2f);

	// turn on lighting 
	enableLighting();

	//turn on the fog in openGL
	glEnable(GL_FOG);

	//sets the type of fog to be linear so its intercity changes with the distance the camera is away in a linear progression
	glFogi(GL_FOG_MODE, GL_LINEAR); 
	glFogf(GL_FOG_START, 5.f); //minimum fog
	glFogf(GL_FOG_END, 100.f); //maximum fog

	//sets the colour of the fog
	float FogCol[3]={0.0f,0.0f,2.0f};
	glFogfv(GL_FOG_COLOR,FogCol);

	//sets the density of the fog
	glFogf(GL_FOG_DENSITY, 0.02f);
	
	//loads image
	cgg::BitmapImage map("map.bmp");

	//loads the bits per pixel, height and width to new variables.
	uint32_t bpp = map.bpp();
	uint32_t height = map.height();
	uint32_t width = map.width();

	//declares the variables needed for the height map
	uint32_t index = 0;
	float heightScale = 0.05f;
	float positionScale = 0.1f;
	float displayScale = 12.8f;
	float colourScale = 255.0f;

	//creates and array for the vertices of the height map
	std::vector<Vec3> mapVertices;
	g_mapPoints.resize(height*width);

	for(float y = 0; y < height; y++) //loop every row
	{ 
		for(float x = 0; x < width; x++)// loop every column
		{ 
				uint32_t offset = (y*width + x) * bpp; //set the offset using the bits per pixel

				/*fill the current vector with the current y and x value and scale and move it to be in a better position 
				and get the value of the colour and use it as the z value and scale it to make it fit better*/
				g_mapPoints[index].v = Vec3( (y * positionScale) - displayScale, (x * positionScale) - displayScale, map.pixels()[offset] * heightScale );

				//set the colour of the map to the colour on the image and divide it by 255 so that the number is between 0 and 1
				g_mapPoints[index].c = Vec3( (map.pixels()[offset]/colourScale) , (map.pixels()[offset]/colourScale), (map.pixels()[offset]/colourScale) );
				
				/*if the current position is not the the last row or the last column then create an indices for the triangle, 
				this check is to stop infinite triangles*/
				if (index < ((width * height) - width) && x < (height - 1))
				{
					// adds the first triangle of the current square
					g_indices.push_back(index); 
					g_indices.push_back(index + 1); 
					g_indices.push_back(index + width); 
					// adds the second triangle of the current square
					g_indices.push_back(index + width); 
					g_indices.push_back(index + 1); 
					g_indices.push_back(index + (width + 1));
				}
				index++;//add one to the current position in the array
			
		}
	}
	//create the normals for all of the triangles
	for(int i = 0; i < (width*height) - width - 1; i++)
	{ 
		if ((i%2) == 0)//work out if the current index is a multiple of two and switch between the needed normal type to get accordingly
		{
			g_mapPoints[i].n = getNormal(g_mapPoints[i].v, g_mapPoints[i + 1].v, g_mapPoints[i + width].v);
		}
		else
		{
			g_mapPoints[i].n = getNormal(g_mapPoints[i + width].v, g_mapPoints[i + 1].v, g_mapPoints[i + width + 1].v);
		}	
	}
}

//------------------------------------------------------------------------------------------------------------------------------------
// called when we need to update the app. 'dt' is the time delta (the number of seconds since the last frame)
//------------------------------------------------------------------------------------------------------------------------------------
void update(float dt)
{
	if(isKeyPressed(kKeyEscape))
	{
		exit(0);
	}
}

//------------------------------------------------------------------------------------------------------------------------------------
// called when we want to draw the 3D data in our app.
//------------------------------------------------------------------------------------------------------------------------------------
void draw3D()
{
	drawPrimitives( g_mapPoints, g_indices, kTriangles);//draw the height map
}

//------------------------------------------------------------------------------------------------------------------------------------
// Called after draw3D. Allows you to draw a 2D overlay.
//------------------------------------------------------------------------------------------------------------------------------------
void draw()
{
	
}

//------------------------------------------------------------------------------------------------------------------------------------
/// called when the left mouse button is pressed
//------------------------------------------------------------------------------------------------------------------------------------
void mousePress(int /*button*/, int x, int y)
{
	Vec2 p = getScreenCoordinates(x, y);
}

//------------------------------------------------------------------------------------------------------------------------------------
/// called when the left mouse button is released
//------------------------------------------------------------------------------------------------------------------------------------
void mouseRelease(int /*button*/, int x, int y)
{
	Vec2 p = getScreenCoordinates(x, y);
}

//------------------------------------------------------------------------------------------------------------------------------------
/// called when the mouse is moved (and the left button is held)
//------------------------------------------------------------------------------------------------------------------------------------
void mouseMove(int x, int y)
{
	Vec2 p = getScreenCoordinates(x, y);
}

//------------------------------------------------------------------------------------------------------------------------------------
/// main  \o/
//------------------------------------------------------------------------------------------------------------------------------------
int main()
{
	// just run the app.... 
	return runApp();
}
