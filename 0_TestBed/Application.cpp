#include "ApplicationClass.h"
void ApplicationClass::InitUserAppVariables()
{
	m_pCamera->SetPosition(vector3(0.0f, 0.0f, 15.0f));

	// random seed, create rand vector positions
	srand((unsigned int)time(NULL));
	float randX;
	float randY;
	float randZ;
	int maxSpawnDistance = 10;
	
	// for each enemy, create a random vector3
	for (int nEnemy = 0; nEnemy < 10; nEnemy ++)
	{
		// randomly places in a cube with volume of maxSpawnDistance^3, but center spawn point is world origin
		randX = (float)(rand() % maxSpawnDistance - maxSpawnDistance/2);
		randY = (float)(rand() % maxSpawnDistance - maxSpawnDistance/2);
		randZ = (float)(rand() % maxSpawnDistance - maxSpawnDistance/2);

		// load the enemy
		m_pMeshMngr->LoadModelUnthreaded("Minecraft\\MC_Cow.obj", "Cow", glm::translate(vector3(randX, randY, randZ)) * glm::scale(vector3(0.5f)));


		//set initial sphere location
		m_m4Sphere = glm::translate(vector3(0.0f,1.0f,15.0f));
	}
}
void ApplicationClass::Update (void)
{
	//time stuff
	m_pSystem->UpdateTime(); //Update the system
	float fTimeSpan = m_pSystem->LapClock(0);//check time difference between method calls
	static float fRunTime = 0.0f;
	fRunTime += fTimeSpan;

//generate random direction until we start using camera direction
	float randX = (float)(rand() % 100);
		float randY = (float)(rand() % 25);
		float randZ = (float)(rand() % 100);

		if(randX > 50)
		{ 
			randX -= 50.0f;
			randX *= -1.0f;
		}

	static vector3 v3direction = vector3(randX,randY,randZ * -1.0f);
	v3direction = glm::normalize(v3direction);
//delete between these comments when camera direction is working

	m_pMeshMngr->SetModelMatrix(m_m4SelectedObject, m_sSelectedObject); //Setting up the Model Matrix
	m_pMeshMngr->Update(); //Update the mesh information

	//will be the direction of camera, scaled by some shit I'll figure out later.
	//vector3 v3direction = vector3();
	//sphere physics calculations
	Physics(fTimeSpan,fRunTime, v3direction);
	

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

void ApplicationClass::Physics(float fTimeSpan, float fRunTime, vector3 v3direction)
{
	matrix4 gravity = glm::translate(vector3(0.0f,fRunTime * -0.1f,0.0f));
	matrix4 translate = glm::translate(fTimeSpan * (v3direction * 30.0f));
	m_m4Sphere = m_m4Sphere * translate * gravity;
	m_pMeshMngr->AddSphereToQueue(m_m4Sphere);
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

	recursiveOctTree(edgeLength, rootCentroid, 5, listObjects);
}

void ApplicationClass::recursiveOctTree(float parentEdgeLength, vector3 parentCentroid, int iteration, std::vector<BoundingObjectClass*> listObjects)
{
	if (iteration > 0)
	{
		iteration--;

		std::vector<BoundingObjectClass*> containedObjects = std::vector<BoundingObjectClass*>();

		// iterate to get 8 subdivs
		vector3 subdivCentroid;
		float subdivEdge = parentEdgeLength/4.0f;
		
		for (int numSubdiv = 0; numSubdiv < 8; numSubdiv++)
		{
			containedObjects.clear();
			subdivCentroid = parentCentroid;

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
			//m_pMeshMngr->AddAxisToQueue(glm::translate(subdivCentroid));
			
			for (int i = 0; i < listObjects.size(); i++)
			{
				if (listObjects[i]->GetCentroidGlobal().x > subdivCentroid.x - subdivEdge &&
					listObjects[i]->GetCentroidGlobal().x < subdivCentroid.x + subdivEdge &&
					listObjects[i]->GetCentroidGlobal().y > subdivCentroid.y - subdivEdge &&
					listObjects[i]->GetCentroidGlobal().y < subdivCentroid.y + subdivEdge &&
					listObjects[i]->GetCentroidGlobal().z > subdivCentroid.z - subdivEdge &&
					listObjects[i]->GetCentroidGlobal().z < subdivCentroid.z + subdivEdge)
				{
					containedObjects.push_back(listObjects[i]);
				}
			}

			if (containedObjects.size() > 0)
			{
				m_pMeshMngr->AddCubeToQueue(glm::translate(subdivCentroid) * glm::scale(vector3(parentEdgeLength / 2.0f)), vector3(0.7f, 0.0f, 0.0f), MERENDER::WIRE);

				if (containedObjects.size() > 1)
				{
					recursiveOctTree(parentEdgeLength / 2.0f, subdivCentroid, iteration, containedObjects);
				}
			}
		}
	}
}