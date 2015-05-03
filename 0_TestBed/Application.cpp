#include "ApplicationClass.h"
#include "OctantCustom.h"

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
		m_m4Sphere = glm::translate(vector3(0.0f,-20.0f,0.0f));
		m_v3Direction = vector3(0.0f,0.0f,0.0f);

		//set shot time high to allow initial shot
		m_fShotTime = 5.0f;
	}

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
	matrix4 gravity = glm::translate(vector3(0.0f,fShotTime * -0.1f,0.0f));//gravity translation
	matrix4 translate = glm::translate(fTimeSpan * (v3direction * 60.0f));//velocity translation
	m_m4Sphere = m_m4Sphere * translate * gravity;//apply translations
	m_pMeshMngr->AddSphereToQueue(m_m4Sphere);//render sphere
}