/*
 * Main.cpp
 *
 *  Created on: Sep 21, 2017
 *      Author: root
 */
#include "Utils/ApplicationManager.h"

int main (int argc, char* argv[])
{
	ApplicationManager manager;

	if (manager.initialize () == false)
	{
		exit (-1);
	}

	manager.pending ();

	exit(0);
}


