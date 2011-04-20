/****************************************************************************
*                                                                           *
*   Nite 1.3 - Boxes Sample                                                 *
*                                                                           *
*   Author:     Oz Magal                                                    *
*                                                                           *
****************************************************************************/

/****************************************************************************
*                                                                           *
*   Nite 1.3	                                                            *
*   Copyright (C) 2006 PrimeSense Ltd. All Rights Reserved.                 *
*                                                                           *
*   This file has been provided pursuant to a License Agreement containing  *
*   restrictions on its use. This data contains valuable trade secrets      *
*   and proprietary information of PrimeSense Ltd. and is protected by law. *
*                                                                           *
****************************************************************************/

#include <XnOpenNI.h>
#include <XnCppWrapper.h>
#include <XnVHandPointContext.h>
#include <XnVSessionManager.h>
#include <XnVFlowRouter.h>
#include <XnVSwipeDetector.h>
#include <XnVSelectableSlider1D.h>
#include <XnVSteadyDetector.h>
#include <XnVBroadcaster.h>
#include <XnVPushDetector.h>

#include <iostream>

        #include <GL/glut.h>

#include "signal_catch.h"
GLfloat verts_box[]={(1),(1),(-1),
                   (-1),(1),(-1),
                   (-1),(-1),(-1),
                   (1),(-1),(-1),
                    (1),(-1),(1),
                    (1),(1),(1),
                    (-1),(1),(1),
                     (-1),(-1),(1)};
                                   // 8 of vertex coords
GLubyte indices_box[] = {0,1,2,3,   // 24 of indices
                     0,3,4,5,
                     0,5,6,1,
                     1,6,7,2,
                     7,4,3,2,
                     4,7,6,5};

int rows=10;
int columns=10;
float yLocation = 0.0f; // Keep track of our position on the y axis.
float xLocation = 0.0f; // Keep track of our position on the x axis.
float zLocation = 0.0f; // Keep track of our position on the z axis.
float xRotationAngle = 0.0f; // The angle of rotation for our object
float yRotationAngle = 0.0f; // The angle of rotation for our object



XnVSessionManager* g_pSessionManager = NULL;
XnVFlowRouter* g_pMainFlowRouter;
XnVPushDetector* g_pMainPush;
XnVSelectableSlider1D* g_pMainSlider;
XnVSwipeDetector* g_pSwipeDetector;

XnBool g_bActive = false;
XnBool g_bIsInput = false;
XnBool g_bInSession = false;
XnFloat g_fValue = 0.5f;

// Main slider
void XN_CALLBACK_TYPE MainSlider_OnHover(XnInt32 nItem, void* cxt)
{
	
}

void XN_CALLBACK_TYPE MainSlider_OnSelect(XnInt32 nItem, XnVDirection dir, void* cxt)
{
	if (nItem == -1)
	{
	}
     
        else if (dir == DIRECTION_BACKWARD)
	{
            zLocation+=5.0;
            printf("Slider: Backward\n");
	}
        else if (dir == DIRECTION_FORWARD)
	{
            
            printf("Slider: Forward\n");
	}

        
	else
	{
		printf("Slider: Bad direction for selection: %s\n", XnVDirectionAsString(dir));
	}
}

void XN_CALLBACK_TYPE MainSlider_OnValueChange(XnFloat fValue, void* cxt)
{
	g_bActive = true;
	g_bIsInput = true;
	g_fValue = fValue;
        yRotationAngle=fValue*360;
         //printf("Slider pos :%f\n",fValue);
}

void XN_CALLBACK_TYPE MainSlider_OnActivate(void* cxt)
{
	g_bActive = true;
	g_bIsInput = false;
}
void XN_CALLBACK_TYPE MainSlider_OnDeactivate(void* cxt)
{
	g_bActive = false;
	g_bIsInput = false;
}
void XN_CALLBACK_TYPE MainSlider_OnPrimaryCreate(const XnVHandPointContext* hand, const XnPoint3D& ptFocus, void* cxt)
{
	g_bIsInput = true;
}
void XN_CALLBACK_TYPE MainSlider_OnPrimaryDestroy(XnUInt32 nID, void* cxt)
{
	g_bIsInput = false;
}

// Swipe detector
	static void XN_CALLBACK_TYPE Swipe_SwipeUp(XnFloat fVelocity, XnFloat fAngle, void* cxt)
	{
		printf("Swipe Up!\n");


                yLocation = 0.0f; // Keep track of our position on the y axis.
                xLocation = 0.0f; // Keep track of our position on the x axis.
zLocation = 0.0f; // Keep track of our position on the z axis.
xRotationAngle = 0.0f; // The angle of rotation for our object
yRotationAngle = 0.0f;


		
	}

	static void XN_CALLBACK_TYPE Swipe_SwipeDown(XnFloat fVelocity, XnFloat fAngle, void* cxt)
	{
		printf("Swipe Down!\n");
	
	}

	static void XN_CALLBACK_TYPE Swipe_SwipeLeft(XnFloat fVelocity, XnFloat fAngle, void* cxt)
	{
	yRotationAngle-=90.0;
            printf("Swipe Left!\n");
		
	}

	static void XN_CALLBACK_TYPE Swipe_SwipeRight(XnFloat fVelocity, XnFloat fAngle, void* cxt)
	{
	yRotationAngle+=90.0;
            printf("Swipe Right!\n");
		
	}

        //push detector callbacks
        static void XN_CALLBACK_TYPE Push_Pushed(XnFloat fVelocity, XnFloat fAngle, void* cxt)
	{
		printf("Push! velocity %f angle %f\n",fVelocity,fAngle);
zLocation-=5.0;

	}

#define GL_WIN_SIZE_X 720
#define GL_WIN_SIZE_Y 480

xn::Context g_Context;
xn::DepthGenerator g_DepthGenerator;


XnBool g_bQuit = false;
void CleanupExit()
{
	if (NULL != g_pSessionManager) {
		delete g_pSessionManager;
		g_pSessionManager = NULL;
	}

	delete g_pMainFlowRouter;
	delete g_pMainSlider;
	

	g_Context.Shutdown();

	exit (1);
}
void renderPrimitive (void) {
   glColor3f(.2,.3,.5);
     glEnableClientState(GL_VERTEX_ARRAY);
glVertexPointer(3, GL_FLOAT, 0, verts_box);
glDrawElements(GL_QUADS, 24, GL_UNSIGNED_BYTE, indices_box);


glBegin(GL_LINES);
glColor3f(1.0,0.0,.0);
  //x axis
       glVertex3f(-10,0,0 );
       glVertex3f(10,0,0 );
    glColor3f(0.0,1.0,.0);
       //y axis
        glVertex3f(0,-10,0 );
       glVertex3f(0,10,0 );
    glColor3f(0.0,0.0,1.0);
       //z-axis
      glVertex3f(0,0,-10 );
       glVertex3f(0,0,10 );


      glEnd();}
// this function is called each frame

void glutDisplay (void)
{
	// Read next available data
	g_Context.WaitAndUpdateAll();

	// Process the data
	g_pSessionManager->Update(&g_Context);
   glClearColor(0.8f,0.8f,0.8f,1.0f);
     glClear(GL_COLOR_BUFFER_BIT);
     glLoadIdentity();//loads identity matrix to reset drawings
      glTranslatef(0.0f,0.0f,-5.0);
     
      glTranslatef(xLocation,0.0f,zLocation);

    glRotatef(yRotationAngle, 0.0f, 1.0f, 0.0f);
     renderPrimitive();
     glFlush();
	
	
}


void glutIdle (void)
{
	if (g_bQuit) {
		CleanupExit();
	}
        glutDisplay();
  
	// Display the frame
	glutPostRedisplay();
}
void glutKeyboard (unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		CleanupExit();
	}
}

 void reshape (int width, int height) {

     glViewport(0, 0, (GLsizei)width, (GLsizei)height); // Set our viewport to the size of our window
    glMatrixMode(GL_PROJECTION); // Switch to the projection matrix so that we can manipulate how our scene is viewed
    glLoadIdentity(); // Reset the projection matrix to the identity matrix so that we don't get any artifacts (cleaning up)
  gluPerspective(120, (GLfloat)width / (GLfloat)height, 1.0, 100.0); // Set the Field of view angle (in degrees), the aspect ratio of our window, and the new and far planes
    glMatrixMode(GL_MODELVIEW); // Switch back to the model view matrix, so that we can start drawing shapes correctly
 }
 
void glInit (int * pargc, char ** argv)
{
	//init both windows
        glutInit(pargc, argv);
	glutInitDisplayMode(GLUT_SINGLE);
	glutInitWindowSize(GL_WIN_SIZE_X, GL_WIN_SIZE_Y);
	//create first window
   
        glutInitWindowPosition (1100, 100);
        glutCreateWindow ("Box");
    
	//glutSetCursor(GLUT_CURSOR_NONE);
	glutKeyboardFunc(glutKeyboard);
	glutReshapeFunc(reshape);
        glutDisplayFunc(glutDisplay);
	glutIdleFunc(glutDisplay);



}


// Callback for the MyBox
void XN_CALLBACK_TYPE MyBox_Leave(void* UserContext)
{
	g_pMainFlowRouter->SetActive(g_pMainSlider);
}

void XN_CALLBACK_TYPE SessionStart(const XnPoint3D& pFocus, void* UserCxt)
{
	g_bInSession = true;
	g_pMainFlowRouter->SetActive(g_pMainSlider);
}
void XN_CALLBACK_TYPE SessionEnd(void* UserCxt)
{
	g_bInSession = false;
	g_pMainFlowRouter->SetActive(NULL);
}



#define CHECK_RC(rc, what)											\
	if (rc != XN_STATUS_OK)											\
{																\
	printf("%s failed: %s\n", what, xnGetStatusString(rc));		\
	return rc;													\
}

#define CHECK_ERRORS(rc, errors, what)		\
	if (rc == XN_STATUS_NO_NODE_PRESENT)	\
{										\
	XnChar strError[1024];				\
	errors.ToString(strError, 1024);	\
	printf("%s\n", strError);			\
	return (rc);						\
}

int main(int argc, char **argv)
{
	XnStatus rc = XN_STATUS_OK;
	xn::EnumerationErrors errors;

	rc = g_Context.InitFromXmlFile("KinConfig.xml");
	CHECK_ERRORS(rc, errors, "InitFromXmlFile");
	CHECK_RC(rc, "InitFromXml");

	rc = g_Context.FindExistingNode(XN_NODE_TYPE_DEPTH, g_DepthGenerator);
	CHECK_RC(rc, "Find depth generator");

	// Create and initialize point tracker
	g_pSessionManager = new XnVSessionManager;
	rc = g_pSessionManager->Initialize(&g_Context, "Wave", "RaiseHand");
	if (rc != XN_STATUS_OK)
	{
		printf("Couldn't initialize the Session Manager: %s\n", xnGetStatusString(rc));
		delete g_pSessionManager;
		return rc;
	}

	g_pSessionManager->RegisterSession(NULL, &SessionStart, &SessionEnd);

	// Start catching signals for quit indications
	CatchSignals(&g_bQuit);

	// Create and initialize the main slider


	g_pMainSlider = new XnVSelectableSlider1D(3,0,AXIS_X,TRUE,.5,250,.05,"slider");
	//g_pMainSlider->RegisterItemHover(NULL, &MainSlider_OnHover);
	g_pMainSlider->RegisterItemSelect(NULL, &MainSlider_OnSelect);
	g_pMainSlider->RegisterActivate(NULL, &MainSlider_OnActivate);
	g_pMainSlider->RegisterDeactivate(NULL, &MainSlider_OnDeactivate);
	g_pMainSlider->RegisterPrimaryPointCreate(NULL, &MainSlider_OnPrimaryCreate);
	g_pMainSlider->RegisterPrimaryPointDestroy(NULL, &MainSlider_OnPrimaryDestroy);
      
	g_pMainSlider->RegisterValueChange(NULL, &MainSlider_OnValueChange);
	g_pMainSlider->SetValueChangeOnOffAxis(true);

        //create swipe detector
        g_pSwipeDetector= new XnVSwipeDetector();
       
                g_pSwipeDetector->RegisterSwipeUp(NULL, &Swipe_SwipeUp);
		g_pSwipeDetector->RegisterSwipeDown(NULL, &Swipe_SwipeDown);
		g_pSwipeDetector->RegisterSwipeLeft(NULL, &Swipe_SwipeLeft);
		g_pSwipeDetector->RegisterSwipeRight(NULL, &Swipe_SwipeRight);
                //std::cout<<"motion speed thresh"<<g_pSwipeDetector->GetMotionSpeedThreshold();
g_pSwipeDetector->SetMotionSpeedThreshold(.15f);

               //std::cout<<"motion speed thresh"<<g_pSwipeDetector->GetMotionSpeedThreshold();
g_pMainPush= new XnVPushDetector();
	g_pMainPush->RegisterPush(NULL, &Push_Pushed);

// Creat the flow manager
	g_pMainFlowRouter = new XnVFlowRouter;

	// Connect flow manager to the point tracker
	g_pSessionManager->AddListener(g_pMainFlowRouter);
        g_pSessionManager->AddListener(g_pSwipeDetector);
        g_pSessionManager->AddListener(g_pMainPush);
	g_Context.StartGeneratingAll();

	

	glInit(&argc, argv);
	glutMainLoop();

	



	CleanupExit();

	
}
