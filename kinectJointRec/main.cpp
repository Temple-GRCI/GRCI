/*****************************************************************************
*                                                                            *
*  OpenNI 1.0 Alpha                                                          *
*  Copyright (C) 2010 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
*                                                                            *
*  OpenNI is free software: you can redistribute it and/or modify            *
*  it under the terms of the GNU Lesser General Public License as published  *
*  by the Free Software Foundation, either version 3 of the License, or      *
*  (at your option) any later version.                                       *
*                                                                            *
*  OpenNI is distributed in the hope that it will be useful,                 *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU Lesser General Public License  *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.            *
*                                                                            *
*****************************************************************************/


/**
 * Gesture Recognition Control Interface (G.R.C.I.)
 *
 * Changes:
 *	* Status updates have been removed to improve readability (PR)
 *	* Added comment blocks for all functions (EO)
 *   * Merged joint recognition with the virtual robot. (PR)
 *   * More documentation (EO)
 *
 * @project     kinectJointRec
 * @author      CIS 3287 - G.R.C.I. Team
 * @link        http://openni.ninesixweb.com
 */


//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include "SceneDrawer.h"
#include "PointDrawer.h"
#include "XnVNite.h"

#include <XnVHandPointContext.h>
#include <XnVSessionManager.h>
#include <XnVFlowRouter.h>
#include <XnVSwipeDetector.h>
#include <XnVSelectableSlider1D.h>
#include <XnVSteadyDetector.h>
#include <XnVBroadcaster.h>
#include <XnVPushDetector.h>


#include <fstream>
#include<stdio.h>
#include<iostream>
#include <GL/glut.h>

#include "pointsInit.h"

#define GL_WIN_SIZE_X 720
#define GL_WIN_SIZE_Y 480

#define SAMPLE_XML_PATH "KinConfig.xml"

#define CHECK_RC(nRetVal, what)										\
	if (nRetVal != XN_STATUS_OK)									\
	{																\
		printf("%s failed: %s\n", what, xnGetStatusString(nRetVal));\
		return nRetVal;												\
	}

//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------


// OpenNI objects
xn::Context			g_Context;			// represents an OpenNI Context Object
xn::DepthGenerator		g_DepthGenerator;
xn::UserGenerator		g_UserGenerator;
xn::HandsGenerator		g_HandsGenerator;
xn::ImageGenerator		g_ImageGenerator;
xn::Recorder			recorder;

// Nite objects
XnVSessionManager*      g_SessionManager= NULL;
XnVFlowRouter*          g_FlowRouter;
XnVSelectableSlider1D*  g_pMainSlider;
XnVSwipeDetector*       g_pSwipeDetector;
XnVFlowRouter*          g_pMainFlowRouter;

// Stuff for the Model
const float ANGLE = 30.0f;
const float PI = 3.14f;

float yLocation = 0.0f; // Keep track of our position on the y axis.
float xLocation = 0.0f; // Keep track of our position on the x axis.
float zLocation = 0.0f; // Keep track of our position on the z axis.
float xRotationAngle = 0.0f; // The angle of rotation for our object
float yRotationAngle = 0.0f; // The angle of rotation for our object
XnFloat g_fValue = 0.5f;

int win1,	// Skeleton
    win2;	// Model

// Session state
XnBool g_bActive = false;
XnBool g_bIsInput = false;
XnBool g_bInSession = false;

// Skeleton detection
XnBool g_bNeedPose = false;
XnChar g_strPose[20] = "";

// Booleans for glwindow
XnBool g_bDrawBackground = true;
XnBool g_bDrawPixels = true;
XnBool g_bDrawSkeleton = true;
XnBool g_bPrintID = true;
XnBool g_bPrintState = true;

// Booleans recording and playback of files
XnBool g_runfile = false; // Use mock node(T) or kinect(F)
XnBool g_Record = false;  // Record a new .oni file(T)
XnBool g_bPause = false;
XnBool g_bRecord = false;

XnBool g_bQuit = false;

std::ofstream outpnt;

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

/**
 * CleanupExit()
 *
 * Tells the context to shut it all down.
 * Clean up recording temp files.
 *
 */

void CleanupExit ()
{
	g_Context.Shutdown();
	rename("recording.tmp.oni", "recording.oni");
	exit (1);
}

/**
 * rad2deg()
 *
 * Converts a radian value to a degree value.
 *
 * @param	radians	float
 * @return	degree	float
 */

float rad2deg (float radians)
{
	return (radians * 180) / PI;
}

/**
 * deg2rad()
 *
 * Converts a degree value to a radian value.
 *
 * @param	degrees	float
 * @return	radian	float
 */

float deg2rad (float degrees)
{
	return (degrees * PI) / 180;
}

/**
 * Session Manager - ( in-session, not in-session, quick-refocus )
 *	in:	tracking the users hand.
 *	not:	looking for focus gesture.
 *	qr:	picks up hand again if it comes back in relative position to where it left
 */

void XN_CALLBACK_TYPE SessionStart (const XnPoint3D& ptFocus, void* UserCxt)
{
  	g_bInSession = true;
     g_pMainFlowRouter->SetActive(g_pMainSlider);
     printf("/t set active/n");
}
void XN_CALLBACK_TYPE SessionEnd (void* UserCxt)
{
 	g_bInSession = false;
     g_pMainFlowRouter->SetActive(NULL);
}

/**
 * User_NewUser() - Callback Function
 *
 * Detects the presense of a new user.
 *
 * @param generator
 * @param nId			XnUserID	The user's number.
 * @param pCookie
 */

void XN_CALLBACK_TYPE User_NewUser (xn::UserGenerator& generator, XnUserID nId, void* pCookie)
{
	printf("New User %d\n", nId);
	// New user found
	if (g_bNeedPose)
	{
		/**
		 * GetPoseDetectionCap()
		 *
		 * Gets a PoseDetectionCapability object for accessing
		 * Pose-Detection functionality (allows a user generator to
		 * recognize when a user is in a specific pose)
		 */

		// starts detection for specified pose and the user id
		g_UserGenerator.GetPoseDetectionCap().StartPoseDetection(g_strPose, nId);
		printf("Looking for Calibration Pose: %s\n",g_strPose);
	}
	else
	{
		g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, true);
	}
}

/**
 * User_LostUser() - Callback Fuction
 *
 * Code to execute when a user is lost.
 *
 * @param generator
 * @param nId			XnUserID	The user's number.
 * @param pCookie
 */

void XN_CALLBACK_TYPE User_LostUser (xn::UserGenerator& generator, XnUserID nId, void* pCookie)
{
	printf("Lost user %d\n", nId);
}

/**
 * UserPose_PoseDetected() - Callback Function
 *
 * Executes when a pose is detected by OpenNI.
 *
 * @param capability
 * @param strPose		XnChar*	Name of the detected pose.
 * @param nId			XnUserID	User who posed.
 * @param pCookie
 */

void XN_CALLBACK_TYPE UserPose_PoseDetected (xn::PoseDetectionCapability& capability,
                                             const XnChar* strPose,
                                             XnUserID nId,
                                             void* pCookie)
{
	printf("Pose %s detected for user %d\n", strPose, nId);
	g_UserGenerator.GetPoseDetectionCap().StopPoseDetection(nId);
	g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, true);
}

/**
 * UserCalibration_CalibrationStart() - Calback Function
 *
 * Started Calibration.
 * TODO: Get this to be automatic when a user enters.
 *
 * @param capability
 * @param nId			XnUserID	The user's number.
 * @param pCookie
 */

void XN_CALLBACK_TYPE UserCalibration_CalibrationStart (xn::SkeletonCapability& capability,
                                                        XnUserID nId,
                                                        void* pCookie)
{
	printf("Calibration started for user %d\n", nId);
}

/**
 * UserCalibration_CalibrationEnd() - Callback Function
 *
 * If calibrated it places a skeleton on the user,
 * otherwise it starts looking for poses again.
 *
 * @param capability
 * @param nId			XnUserID	The user's number.
 * @param bSuccess		XnBool	True if calibration succeeded.
 * @param pCookie
 */

void XN_CALLBACK_TYPE UserCalibration_CalibrationEnd (xn::SkeletonCapability& capability,
                                                      XnUserID nId,
                                                      XnBool bSuccess,
                                                      void* pCookie)
{
	if (bSuccess)
	{
		// Calibration succeeded
		printf("Calibration complete, start tracking user %d\n", nId);
		g_UserGenerator.GetSkeletonCap().StartTracking(nId);
	}
	else
	{
		// Calibration failed
		printf("Calibration failed for user %d\n", nId);
		if (g_bNeedPose)
		{
			g_UserGenerator.GetPoseDetectionCap().StartPoseDetection(g_strPose, nId);
		}
		else
		{
			g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, true);
		}
	}
}

/**
 * MainSlider_OnValueChange() - Callback Function
 *
 * If the slider is active, this function will move the virtual
 * robot forward its current direction.
 *
 * Think of this as the gas pedal of our robot.
 *
 * @param fValue	XnFloat		value returned from the slider | 0 < fValue < 1
 * @param cxt		void*		unused
 */

void XN_CALLBACK_TYPE MainSlider_OnValueChange (XnFloat fValue, void* cxt)
{
	g_bActive = true;
	g_bIsInput = true;
	g_fValue = fValue;

	//  range (0, 0.5) = Foward
	if (fValue<0.5)
	{
		yLocation+=((fValue-.5)/10) * sin(deg2rad(yRotationAngle));
		xLocation+=((fValue-.5)/10) * cos(deg2rad(yRotationAngle));
	}
}

/**
 * MainSlider_OnActive() - Callback Function
 *
 *
 * @param cxt	void*
 */

void XN_CALLBACK_TYPE MainSlider_OnActivate (void* cxt)
{
	g_bActive = true;
	g_bIsInput = false;
}

/**
 * MainSlider_OnDeactivate() - Callback Function
 *
 * @param cxt	void*
 */

void XN_CALLBACK_TYPE MainSlider_OnDeactivate (void* cxt)
{
	g_bActive = false;
	g_bIsInput = true;
}

/**
 * MainSlider_OnPrimaryCreate() - Callback Function
 *
 *
 * @param hand		XnVHandPointContext*		unused
 * @param ptFocus	XnPoint3D&				unused
 * @param cxt		void*					unused
 */

void XN_CALLBACK_TYPE MainSlider_OnPrimaryCreate (const XnVHandPointContext* hand,
                                                  const XnPoint3D& ptFocus,
                                                  void* cxt)
{
	g_bIsInput = true;
}

/**
 * MainSlider_OnPrimaryDestroy() - Callback Function
 *
 *
 * @param nID	XnUInt32		User ID (unused)
 * @param cxt	void*		unused
 */

void XN_CALLBACK_TYPE MainSlider_OnPrimaryDestroy (XnUInt32 nID, void* cxt)
{
	g_bIsInput = false;
}

/**
 * Swipe_SwipeUp() - Callback Function
 *
 * This function is used to reset our virtual robot to
 * its starting location.
 *
 * @param fVelocity		XnFloat
 * @param fAngle		XnFloat
 * @param cxt			void*
 */

static void XN_CALLBACK_TYPE Swipe_SwipeUp (XnFloat fVelocity, XnFloat fAngle, void* cxt)
{
	printf("Swipe Up!... RESET\n");
	yLocation = 0.0f;		// Keep track of our position on the y axis.
	xLocation = 0.0f;		// Keep track of our position on the x axis.
	zLocation = 0.0f;		// Keep track of our position on the z axis.
	xRotationAngle = 0.0f;	// The angle of rotation for our object
	yRotationAngle = 0.0f;
}

/**
 * Swipe_SwipeLeft() - Callback Function
 *
 * This function will turn the robot left the defined angle.
 * If the geture is performed with an increased velocity, the
 * robot will be turned left again by the defined angle.
 *
 * @param fVelocity		XnFloat
 * @param fAngle		XnFloat
 * @param cxt			void*
 */

static void XN_CALLBACK_TYPE Swipe_SwipeLeft (XnFloat fVelocity, XnFloat fAngle, void* cxt)
{
	yRotationAngle -= ANGLE;
	if (fVelocity > 0.35)
	{
		yRotationAngle -= ANGLE;
	}
	printf("Swipe Left! Velocity %f\n", fVelocity);
}

/**
 * Swipe_SwipeRight() - Callback Function
 *
 * This function will turn the robot right the defined angle.
 * If the geture is performed with an increased velocity, the
 * robot will be turned right again by the defined angle.
 *
 * @param fVelocity		XnFloat
 * @param fAngle		XnFloat
 * @param cxt			void*
 */

static void XN_CALLBACK_TYPE Swipe_SwipeRight (XnFloat fVelocity, XnFloat fAngle, void* cxt)
{
	yRotationAngle += ANGLE;
	if (fVelocity > 0.35)
	{
		yRotationAngle += ANGLE;
	}
	printf("Swipe Right! Velocity %f\n", fVelocity);
}

/**
 * drawCube()
 *
 * Takes in three matrices required to draw a cube in OpenGL.
 *
 * @param vertices	GLfloat[8][3]	Matrix of 3D points for each vertex.
 * @param indices	GLubyte[6][4]	Matrix for connecting the vertices into faces.
 * @param colors	GLfloat[6][3]	Matrix of RGB values for each of the faces.
 */

void drawCube (GLfloat vertices[8][3],
               GLubyte indices[6][4],
               GLfloat colors[6][3])
{
	glBegin(GL_QUADS);

	// loop for each side
	for(int s=0; s < 6; s++)
	{
		// set the color of the face
		glColor3f(colors[0][0], colors[0][1], colors[0][2]);

		// loop through all the indices
		for(int i=0; i < 4; i++)
		{
			int ind = indices[s][i];
			// draw the vertex used for this face
			glVertex3f(vertices[ind][0], vertices[ind][1], vertices[ind][2]);
		}
	}

	glEnd();
}

/**
 * drawRobot()
 *
 * Makes a series of calls to drawCube() to draw
 * the robot parts.
 *		Body
 *		Head
 *		Left Arm
 *		Right Arm
 */

void drawRobot ()
{
	drawCube(vertices_robody, indices_cube, color_robody);
	drawCube(vertices_rohead, indices_cube, color_rohead);
	drawCube(vertices_roleftarm, indices_cube, color_roleftarm);
	drawCube(vertices_rorightarm, indices_cube, color_rorightarm);
}

/**
 * renderPrimitive()
 *
 * Draws the robot and the X,Y,Z axis.
 * Called by glutDisplay().
 */

void renderPrimitive (void) {

	// what do you think this does?
	drawRobot();

	//draw axis lines
	glBegin(GL_LINES);

	// x-axis // red
	glColor3f(1.0,0.0,.0);
	glVertex3f(-10,0,0);
	glVertex3f(10,0,0);

	// y-axis // green
	glColor3f(0.0,1.0,.0);
	glVertex3f(0,-10,0);
	glVertex3f(0,10,0);

	// z-axis // blue
	glColor3f(0.0,0.0,1.0);
	glVertex3f(0,0,-10);
	glVertex3f(0,0,10);

	glEnd();
}

/**
 * reshape() - For glutReshapeFunc()
 *
 * Boilerplate for setting up the GL environment.
 *
 */

void reshape (int width, int height) {

	// Set our viewport to the size of our window
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	// Switch to the projection matrix so that we can manipulate how our scene is viewed
	glMatrixMode(GL_PROJECTION);

	// Reset the projection matrix to the identity matrix so that we don't get any artifacts (cleaning up)
	glLoadIdentity();

	// Set the Field of view angle (in degrees), the aspect ratio of our window, and the new and far planes
	gluPerspective(100, (GLfloat)width / (GLfloat)height, 1.0, 100.0);

	// Switch back to the model view matrix, so that we can start drawing shapes correctly
	glMatrixMode(GL_MODELVIEW);
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      GL Functions for Skeleton                                            //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



/**
 * glutDisplays() - (Skeleton)
 *
 * This gets passed to glut for displaying each frame.
 *
 * @param void
 */

void glutDisplayS (void)
{
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Setup the OpenGL viewpoint
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	xn::SceneMetaData sceneMD;//Represents a MetaData object for generators producing scene analysis maps
	xn::DepthMetaData depthMD;
	g_DepthGenerator.GetMetaData(depthMD);
	glOrtho(0, depthMD.XRes(), depthMD.YRes(), 0, -1.0, 1.0);

	glDisable(GL_TEXTURE_2D);

	if (!g_bPause)
	{
		// Read next available data
		g_Context.WaitAnyUpdateAll();
		if(g_Record)
		{
			recorder.Record();
		}
	}

	// Process the data
	g_DepthGenerator.GetMetaData(depthMD);
	// Get the pixels that belong to a user
	g_UserGenerator.GetUserPixels(0, sceneMD);
	DrawDepthMap(depthMD, sceneMD);

	// Swaps the buffers of the current window if double buffered.
	glutSwapBuffers();
}

/**
 * glutKeyboardS() - (Skeleton)
 *
 * Perform actions based off of what key the user presses.
 * This gets passed to glut to handle input.
 *
 * **** Only recieves input for keys pressed while in the skeleton window.
 *
 * @param key	unsigned char	name/value of the key pressed
 * @param x		int				purpose unknown (Possibly mouse coords)
 * @param y		int				purpose unknown
 */

void glutKeyboardS (unsigned char key, int x, int y)
{
	switch (key)
	{
		case 27:
			// Escape
			CleanupExit();
		case 'b':
			// Draw background?
			g_bDrawBackground = !g_bDrawBackground;
			break;
		case 'x':
			// Draw pixels at all?
			g_bDrawPixels = !g_bDrawPixels;
			break;
		case 's':
			// Draw Skeleton?
			g_bDrawSkeleton = !g_bDrawSkeleton;
			break;
		case 'i':
			// Print label?
			g_bPrintID = !g_bPrintID;
			break;
		case 'l':
			// Print ID & state as label, or only ID?
			g_bPrintState = !g_bPrintState;
			break;
		case'p':
			g_bPause = !g_bPause;
			break;
		case'r':
			g_bRecord = !g_bRecord;
			if (!g_bRecord)
			{
				outpnt << "$\n";
			}
			printf("Record on/off\n");
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                        GL Functions for Model                                             //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * glutDisplayM - (Model)
 *
 *
 *
 *
 */

void glutDisplayM (void)
{
	// Read next available data
	g_Context.WaitNoneUpdateAll();

	// Process the data
	g_SessionManager->Update(&g_Context);
	glClearColor(0.8f,0.8f,0.8f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
     glLoadIdentity();//loads identity matrix to reset drawings
	glTranslatef(0.0f,0.0f,-15.0);
	glTranslatef(xLocation,yLocation, zLocation);
	glRotatef(yRotationAngle, 0.0f, 0.0f, 1.0f);
     renderPrimitive();
     glFlush();
}

/**
 * glutKeyboardM() - (Model)
 *
 * Perform actions based off of what key the user presses.
 * This gets passed to glut to handle input.
 *
 * **** Only recieves input for keys pressed while in the model window.
 *
 * @param key		unsigned char		name/value of the key pressed
 * @param x		int				purpose unknown (Possibly mouse coords)
 * @param y		int				purpose unknown
 */

void glutKeyboardM (unsigned char key, int x, int y)
{

	switch (key)
	{
		case 'a':
			yRotationAngle += ANGLE;
			break;
		case 'd':
			yRotationAngle -= ANGLE;
			break;
		case 'w':
			MainSlider_OnValueChange(1, NULL);
			break;
		case 'z':
			MainSlider_OnValueChange(0, NULL);
			break;
		case 27:
			CleanupExit();
	}
}

/**
 * glutIdleSM()
 *
 * 
 *
 *
 */

void glutIdleSM (void)
{
	if (g_bQuit)
	{
		CleanupExit();
	}
	glutSetWindow(win2);
	glutDisplayM();

	// Display the frame
	glutPostRedisplay();
	glutSetWindow(win1);
	glutPostRedisplay();
}

/**
 * glInit()
 *
 * Configuration for glut.
 *
 * @param pargc	int*			points to the number of command line args.
 * @param argv		char**		points to the command line args.
 */

void glInit (int * pargc, char ** argv)
{
	glutInit(pargc, argv);

	///////////////// window 1 //////////////////////////////
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(GL_WIN_SIZE_X, GL_WIN_SIZE_Y);
	win1=glutCreateWindow ("User Tracker");
	glutSetCursor(GLUT_CURSOR_NONE);

	glutKeyboardFunc(glutKeyboardS);
	glutDisplayFunc(glutDisplayS);
	glutIdleFunc(glutIdleSM);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);


	//////////////// window 2 ///////////////////////
	glutInitDisplayMode(GLUT_SINGLE);
	glutInitWindowSize(GL_WIN_SIZE_X, GL_WIN_SIZE_Y);
	//create first window

	glutInitWindowPosition (0, 700);
	win2 = glutCreateWindow ("Box");


	glutKeyboardFunc(glutKeyboardM);
	glutReshapeFunc(reshape);
	glutDisplayFunc(glutDisplayM);
	glutIdleFunc(glutIdleSM);
}

int main(int argc, char **argv)
{
	XnStatus nRetVal = XN_STATUS_OK;

	// Handles to a registered callback function
	XnCallbackHandle hUserCallbacks, hCalibrationCallbacks, hPoseCallbacks;

	outpnt.open("kindat"); // openfile to save data
	std::cout << "\n\n\nKEYBOARD TOGGLE:(focus must be on GL window)\n b-background \n "
                 "x-draw all pixels \n s-Skeleton \n i-print labels \n l- print state \n "
                 "p-pause \n r-record data points\n";

	// Must be called before any OpenNI function ( except xnInitFromXmlFile() )
	nRetVal = g_Context.InitFromXmlFile("KinConfig.xml");
	CHECK_RC(nRetVal, "InitFromXml");
	
	// Returns the first found existing node of the specified type(pointer to context. ,type,handle to node)
	nRetVal = g_Context.FindExistingNode(XN_NODE_TYPE_DEPTH, g_DepthGenerator);
	nRetVal = g_Context.FindExistingNode(XN_NODE_TYPE_USER, g_UserGenerator);
	nRetVal = g_Context.FindExistingNode(XN_NODE_TYPE_HANDS, g_HandsGenerator);
	CHECK_RC(nRetVal, "Find Existing Node");

	if (g_Record)
	{
		recorder.Create(g_Context);     // creates a recorder node
		recorder.SetDestination(XN_RECORD_MEDIUM_FILE,"recording.tmp.oni");
		recorder.AddNodeToRecording(g_DepthGenerator);
	}

	// Create and initialize point tracker
	g_SessionManager = new XnVSessionManager;
	g_SessionManager->Initialize(&g_Context, "Wave", "RaiseHand");
	g_SessionManager->RegisterSession(NULL, &SessionStart, &SessionEnd);

	// Create and initialize the main slider
	g_pMainSlider = new XnVSelectableSlider1D(1,0,AXIS_Z,false,.5,300,150,"slider");
	g_pMainSlider->RegisterActivate(NULL, &MainSlider_OnActivate);
	g_pMainSlider->RegisterDeactivate(NULL, &MainSlider_OnDeactivate);
	g_pMainSlider->RegisterPrimaryPointCreate(NULL, &MainSlider_OnPrimaryCreate);
	g_pMainSlider->RegisterPrimaryPointDestroy(NULL, &MainSlider_OnPrimaryDestroy);
	g_pMainSlider->RegisterValueChange(NULL, &MainSlider_OnValueChange);
	g_pMainSlider->SetValueChangeOnOffAxis(true);

	// Create swipe detector
	g_pSwipeDetector = new XnVSwipeDetector();
	g_pSwipeDetector->RegisterSwipeUp(NULL, &Swipe_SwipeUp);
	g_pSwipeDetector->RegisterSwipeLeft(NULL, &Swipe_SwipeLeft);
	g_pSwipeDetector->RegisterSwipeRight(NULL, &Swipe_SwipeRight);

	// Sets sensitivity of swipe recognition
	g_pSwipeDetector->SetMotionTime(700);		// how long the gesture needs to be to recognize
	g_pSwipeDetector->SetXAngleThreshold(45);
	g_pSwipeDetector->SetSteadyDuration(60);	// how long the hand has to be stable

	// Create the flow manager
	g_pMainFlowRouter = new XnVFlowRouter;

	// Connect flow manager to the point tracker
	g_SessionManager->AddListener(g_pMainFlowRouter);
	g_SessionManager->AddListener(g_pSwipeDetector);


	/**
	 * Register[____]Callbacks()
	 *
	 * For your information.
	 *
	 * @param detection start		XnCallbackHandle
	 * @param detection end			XnCallbackHandle
	 * @param null					pCookie
	 * @param callback handle		XnCallbackHandle
	 */

	g_UserGenerator.RegisterUserCallbacks(User_NewUser, User_LostUser, NULL, hUserCallbacks);
	g_UserGenerator.GetSkeletonCap().RegisterCalibrationCallbacks(
			UserCalibration_CalibrationStart,
			UserCalibration_CalibrationEnd,
			NULL,
			hCalibrationCallbacks
	);

	if (g_UserGenerator.GetSkeletonCap().NeedPoseForCalibration())
	{
		g_bNeedPose = true;
		if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION))
		{
			printf("Pose required, but not supported\n");
			return 1;
		}
		g_UserGenerator.GetPoseDetectionCap().RegisterToPoseCallbacks(
				UserPose_PoseDetected,
				NULL,
				NULL,
				hPoseCallbacks
		);
		g_UserGenerator.GetSkeletonCap().GetCalibrationPose(g_strPose);
	}

	g_UserGenerator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);
	g_Context.StartGeneratingAll(); // Make sure all generators are generating data.


	glInit(&argc, argv);
	glutMainLoop();

	outpnt.close();
}