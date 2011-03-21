// g++ -o SimplerNiViewer SimplerNiViewer.cpp -I/usr/include/ni/ -L/usr/libs/ -l'OpenNI' -l'glut'
// set additional g++ compiler libraries to /usr/include/ni
// set additional linker libraries to /user/lib
// add -l'OpenNI' -l'glut' to to command line options;


#include <XnCppWrapper.h>

#if (XN_PLATFORM == XN_PLATFORM_MACOSX)
	#include <GLUT/glut.h>
	#include <OpenGL/glu.h>
#else
	#include <GL/glut.h>
	#include <GL/glu.h>
#endif


XnStatus nRetVal = XN_STATUS_OK;
xn::Context context;		// The context object stores information about the production nodes you are using
xn::DepthGenerator depth;	// Depth generator object

void drawPoints(void)
{
	const XnDepthPixel* pDepthMap = depth.GetDepthMap();	// The XnDepthPixel pointer points to the beginning of a 1 dimensional block of memory that contains the depth information.
															// If you have a 640x480 depth generator, then it has 307200 values.

	float count = 10.0;
	for (int i = 0; i < (480); i++) // The depth data is organized as if you were reading from left to right, top to bottom of a 640x480 screen.
	{						
		for (int j = 0; j < 640; j++, count+=1.0)
		{
			float pixelvalue = (float)(*(pDepthMap+(int)(count)));
			float colorvalue = (float)(pixelvalue/3000.0);		// lower the divsor = less depth detail, too high and colors become dark(3-5k range)
			if (colorvalue!=0.0)
			{
				glBegin(GL_POINTS);
				glColor4f(5.0f, colorvalue, 0.0f,colorvalue);	// creates a (R,G,B,alpha) alpha == intensitiy
				glVertex3f((float)j, (float)i, 0.0f);			// set the location of the point to be plotted
				glEnd();
             }
        }
	}

}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	nRetVal = context.WaitOneUpdateAll(depth); // This call is what actually updates the depth information

	if (nRetVal != XN_STATUS_OK)
	{
		printf("Failed updating data: %s\n",
		xnGetStatusString(nRetVal));
	}

	drawPoints();

	glutSwapBuffers();
	glutPostRedisplay();
}

void init(void)
{
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_MODELVIEW);
	glOrtho(0.0, 640.0, 480.0, 0.0, 1.0, -1.0);
}

int main(int argc, char **argv)
{

	nRetVal = context.Init();			// initializing the context is necessary to begin getting depth data
	nRetVal = depth.Create(context);	// Creates the depth generating node in the context
	if (nRetVal != XN_STATUS_OK) 
	{
		printf("Failed getting depth node: %s\n",
		xnGetStatusString(nRetVal));

	}

	nRetVal = context.StartGeneratingAll(); // begin recieving depth data
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(640, 480);
	glutCreateWindow("kinect depth data");
	glutDisplayFunc(display);
	init();
	glutMainLoop();
	return 0;
}
