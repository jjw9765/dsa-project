#include "ApplicationClass.h"
#include "OctantCustom.h"
#include <vector>

void ApplicationClass::InitUserAppVariables()
{
	m_pCamera->SetPosition(vector3(0.0f, 0.0f, 15.0f));

	// random seed, create rand vector positions
	srand((unsigned int)time(NULL));
	float randX;
	float randY;
	float randZ;
	int maxSpawnDistance = 20;
	std::vector<vector3> enemyLocs;
	
	// for each enemy, create a random vector3
	for (int nEnemy = 0; nEnemy < 10; nEnemy ++)
	{
		// randomly places in a cube with volume of maxSpawnDistance^3, but center spawn point is world origin
		randX = (float)(rand() % maxSpawnDistance - maxSpawnDistance/2);
		randY = (float)(rand() % maxSpawnDistance - maxSpawnDistance/2);
		randZ = (float)(rand() % maxSpawnDistance - maxSpawnDistance/2);
		enemyLocs.push_back(vector3(randX, randY, randZ));

		//Make sure enemies aren't placed inside eachother
		if(nEnemy > 0)
		{
			for(int j = 0; j < enemyLocs.size(); j++)
			{
				if(randX <= enemyLocs[j].x + 150 && randX >= enemyLocs[j].x - 150)
					randX = (float)(rand() % maxSpawnDistance - maxSpawnDistance/2);

				if(randY <= enemyLocs[j].y + 150 && randY >= enemyLocs[j].y - 150)
					randY = (float)(rand() % maxSpawnDistance - maxSpawnDistance/2);

				if(randZ <= enemyLocs[j].z + 150 && randZ >= enemyLocs[j].z - 150)
					randZ = (float)(rand() % maxSpawnDistance - maxSpawnDistance/2);
			}
		}

		// load the enemy
		m_pMeshMngr->LoadModelUnthreaded("Minecraft\\MC_Cow.obj", "Cow", glm::translate(vector3(randX, randY, randZ)));
	}

	//set initial sphere location
	m_m4Sphere = glm::translate(vector3(0.0f,-20.0f,0.0f));
	m_v3SphereCentroid = vector3(0.0f,-20.0f,0.0f);
	m_v3Direction = vector3(0.0f,0.0f,0.0f);

	//set shot time high to allow initial shot
	m_fShotTime = 5.0f;

	// Create a vector holding all objects in the scene
	int numInstances = m_pMeshMngr->GetNumberOfInstances();
	for (int i = 0; i < numInstances; i++)
		listObjects.push_back(m_pMeshMngr->GetBoundingObject(i));
}
void ApplicationClass::Update (void)
{
	// time stuff
	m_pSystem->UpdateTime(); //Update the system
	m_pMeshMngr->SetModelMatrix(m_m4SelectedObject, m_sSelectedObject); //Setting up the Model Matrix
	m_pMeshMngr->Update(); //Update the mesh information

	float fTimeSpan = m_pSystem->LapClock(0);//check time difference between method calls
	//static float fRunTime = 0.0f;
	//fRunTime += fTimeSpan;
	
	m_fShotTime += fTimeSpan;//increment time since last shot

	//First person camera movement
	if(m_bFPC == true)
		CameraRotation();

	if(m_bArcBall == true)
	{
		ArcBall();
		m_pMeshMngr->SetModelMatrix(m_m4SelectedObject, m_sSelectedObject); //Setting up the Model Matrix
	}

	//sphere physics calculations
	Physics(fTimeSpan,m_fShotTime, m_v3Direction);

	// OctTree this game up
	OctantCustom* newOctTree = new OctantCustom(m_pMeshMngr, listObjects);

	printf("FPS: %d\r", m_pSystem->FPS);//print the Frames per Second	
}

void ApplicationClass::Physics(float fTimeSpan, float fShotTime, vector3 v3direction)
{
	vector3 tempGrav = vector3(0.0f,fShotTime * -0.1f,0.0f);
	vector3 tempVel = fTimeSpan * (v3direction * 60.0f);
	m_v3SphereCentroid = m_v3SphereCentroid + tempGrav + tempVel;

	matrix4 gravity = glm::translate(tempGrav);//gravity translation
	matrix4 translate = glm::translate(tempVel);//velocity translation
	m_m4Sphere = m_m4Sphere * translate * gravity;//apply translations
	m_pMeshMngr->AddSphereToQueue(m_m4Sphere,vector3(0.0f,0.0f,0.0f), 1);//render sphere
	//m_pMeshMngr->AddAxisToQueue(glm::translate(m_v3SphereCentroid));//render centroid vector for debugging
}