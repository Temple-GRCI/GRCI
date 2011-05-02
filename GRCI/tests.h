/*
 *  tests.h
 */

#ifndef RUNTESTS_H
#define	RUNTESTS_H

#include <stdio.h>
#include <string>

bool assertEquals(float expected, float actual, char* message)
{
	if (expected != actual)
	{
		printf("\n-------------\nTEST FAILED\n-------------\n%s | actual = %f\n", message, actual);
		return false;
	}
	else
	{
		printf("\n-------------\nTEST PASSED\n-------------\n");
		return true;
	}
}

void run_tests()
{
	assertEquals(3,4, "3 should equal 3");
	
	printf("deg2rad(30) = %f\n", deg2rad(30));
	printf("rad2deg(0.523598776) = %f\n", rad2deg(0.523598776));
	
	assertEquals(30, rad2deg(0.523598776), "30deg = 0.523598776rad");
	assertEquals(0.523598776, deg2rad(30), "30deg = 0.523598776rad");
}

#endif