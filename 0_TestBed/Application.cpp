#include "ApplicationClass.h"
void ApplicationClass::InitUserAppVariables()
{
	m_pCamera->SetPosition(vector3(0.0f, 0.0f, 15.0f));

	// random seed, create rand vector positions
	srand((unsigned int)time(NULL));
	float randX;
	float randY;
	float randZ;
	
	// for each enemy, create a random vector3
	for (int nEnemy = 0; nEnemy  < 7; nEnemy ++)
	{
		randX = (float)(rand() % 6 - 3);
		randY = (float)(rand() % 6 - 3);
		randZ = (float)(rand() % 6 - 3);

		// load the enemy
		m_pMeshMngr->LoadModelUnthreaded("Minecraft\\MC_Cow.obj", "Cow", glm::translate(vector3(randX, randY, randZ)));
	}
}
void ApplicationClass::Update (void)
{
	m_pSystem->UpdateTime(); //Update the system
	m_pMeshMngr->SetModelMatrix(m_m4SelectedObject, m_sSelectedObject); //Setting up the Model Matrix
	m_pMeshMngr->Update(); //Update the mesh information

	// OctTree this game up
	OctTree();

	//First person camera movement
	if(m_bFPC == true)
		CameraRotation();

	if(m_bArcBall == true)
	{
		ArcBall();
		m_pMeshMngr->SetModelMatrix(m_m4SelectedObject, m_sSelectedObject); //Setting up the Model Matrix
	}

	printf("FPS: %d\r", m_pSystem->FPS);//print the Frames per Second	
}

void ApplicationClass::OctTree (void)
{
	// create variables and the likes
	std::vector<BoundingObjectClass*> listObjects;
	int numInstances = m_pMeshMngr->GetNumberOfInstances();
	vector3 rootMin;
	vector3 rootMax;
	vector3 rootCentroid;

	// populate the vector with all objects
	for (int i = 0; i < numInstances; i++)
		listObjects.push_back(m_pMeshMngr->GetBoundingObject(i));

	rootMin = listObjects[0]->GetCentroidGlobal() - listObjects[0]->GetHalfWidth();
	rootMax = listObjects[0]->GetCentroidGlobal() + listObjects[0]->GetHalfWidth();

	// find the min/max of the root cube by going through each object and calculating its bounds
	for (BoundingObjectClass* boInstance : listObjects)
	{
		if (rootMin.x > boInstance->GetCentroidGlobal().x - boInstance->GetHalfWidth().x)
			rootMin.x = boInstance->GetCentroidGlobal().x - boInstance->GetHalfWidth().x;
		else if (rootMax.x < boInstance->GetCentroidGlobal().x + boInstance->GetHalfWidth().x)
			rootMax.x = boInstance->GetCentroidGlobal().x + boInstance->GetHalfWidth().x;

		if (rootMin.y > boInstance->GetCentroidGlobal().y - boInstance->GetHalfWidth().y)
			rootMin.y = boInstance->GetCentroidGlobal().y - boInstance->GetHalfWidth().y;
		else if (rootMax.y < boInstance->GetCentroidGlobal().y + boInstance->GetHalfWidth().y)
			rootMax.y = boInstance->GetCentroidGlobal().y + boInstance->GetHalfWidth().y;

		if (rootMin.z > boInstance->GetCentroidGlobal().z - boInstance->GetHalfWidth().z)
			rootMin.z = boInstance->GetCentroidGlobal().z - boInstance->GetHalfWidth().z;
		else if (rootMax.z < boInstance->GetCentroidGlobal().z + boInstance->GetHalfWidth().z)
			rootMax.z = boInstance->GetCentroidGlobal().z + boInstance->GetHalfWidth().z;
	}

	// find the centroid of the root cube
	rootCentroid = (rootMin + rootMax) / 2.0f;

	// this is the edge length of the root cube
	float edgeLength = 0;

	// calculate that shit
	if (edgeLength < glm::distance(vector3(rootMin.x, 0, 0), vector3(rootMax.x, 0, 0)))
		edgeLength = glm::distance(vector3(rootMin.x, 0, 0), vector3(rootMax.x, 0, 0));
	if (edgeLength < glm::distance(vector3(rootMin.y, 0, 0), vector3(rootMax.y, 0, 0)))
		edgeLength = glm::distance(vector3(rootMin.y, 0, 0), vector3(rootMax.y, 0, 0));
	if (edgeLength < glm::distance(vector3(rootMin.z, 0, 0), vector3(rootMax.z, 0, 0)))
		edgeLength = glm::distance(vector3(rootMin.z, 0, 0), vector3(rootMax.z, 0, 0));

	// OctTree Root Framework
	m_pMeshMngr->AddAxisToQueue(glm::translate(rootCentroid));
	m_pMeshMngr->AddCubeToQueue(glm::translate(rootCentroid) * glm::scale(vector3(edgeLength)), vector3(1.0f, 1.0f, 1.0f), MERENDER::WIRE);

	// iterate to get 8 subdivs
	vector3 subdivCentroid;
	float subdivEdge = edgeLength/4.0f;
	for (int numSubdiv = 0; numSubdiv < 8; numSubdiv++)
	{
		subdivCentroid = rootCentroid;

		// subdivs are described by how you face them
		switch(numSubdiv)
		{
			case 0:	//	Front Top Right
				subdivCentroid += vector3(subdivEdge);
				break;
			case 1:	//	Front Top Left
				subdivCentroid += vector3(-subdivEdge, subdivEdge, subdivEdge);
				break;
			case 2:	//	Front Bottom Left
				subdivCentroid += vector3(-subdivEdge, -subdivEdge, subdivEdge);
				break;
			case 3:	//	Front Bottom Right
				subdivCentroid += vector3(subdivEdge, -subdivEdge, subdivEdge);
				break;
			case 4:	//	Rear Top Right
				subdivCentroid += vector3(subdivEdge, subdivEdge, -subdivEdge);
				break;
			case 5:	//	Rear Top Left
				subdivCentroid += vector3(-subdivEdge, subdivEdge, -subdivEdge);
				break;
			case 6:	//	Rear Bottom Left
				subdivCentroid -= vector3(subdivEdge);
				break;
			case 7:	//	Rear Bottom Right
				subdivCentroid += vector3(subdivEdge, -subdivEdge, -subdivEdge);
				break;
		}

		// add to render queue
		m_pMeshMngr->AddAxisToQueue(glm::translate(subdivCentroid));
		m_pMeshMngr->AddCubeToQueue(glm::translate(subdivCentroid) * glm::scale(vector3(edgeLength/2.0f)), vector3(0.7f, 0.0f, 0.0f), MERENDER::WIRE);
	}
}


