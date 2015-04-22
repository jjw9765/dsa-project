#include "ApplicationClass.h"
void ApplicationClass::InitUserAppVariables()
{
	m_pCamera->SetPosition(vector3(0.0f, 0.0f, 15.0f));

	m_pMeshMngr->LoadModelUnthreaded("Minecraft\\MC_Steve.obj", "Steve");

	for (int nCreepers = 0; nCreepers  < 20; nCreepers ++)
	{
		m_pMeshMngr->LoadModelUnthreaded("Minecraft\\MC_Creeper.obj", "Creeper", glm::translate(glm::sphericalRand(5.0f)));
	}
	
}
void ApplicationClass::Update (void)
{
	m_pSystem->UpdateTime(); //Update the system
	m_pMeshMngr->SetModelMatrix(m_m4SelectedObject, m_sSelectedObject); //Setting up the Model Matrix
	m_pMeshMngr->Update(); //Update the mesh information


	/*
	vector3 minVec;
	vector3 maxVec;

	std::vector<vector4> v4CollidingObjects = m_pMeshMngr->GetCollisionList();

	unsigned int nObjects = v4CollidingObjects.size();

	for (unsigned int n = 0; n < nObjects; n++)
	{
	
	}

	BoundingObjectClass* pBO = m_pMeshMngr->GetBoundingObject(0);
	pBO->GetCentroidLocal();*/

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
	//std::vector<BoundingObjectClass*>name;
	vector3 min = m_pMeshMngr->GetBoundingObject(0)->GetCentroidGlobal();
	vector3 max = m_pMeshMngr->GetBoundingObject(0)->GetCentroidGlobal();
	
	for (int i = 1; i < m_pMeshMngr->GetNumberOfInstances(); i++)
	{
		if (min.x > m_pMeshMngr->GetBoundingObject(i)->GetCentroidGlobal().x)
		{
			min.x = m_pMeshMngr->GetBoundingObject(i)->GetCentroidGlobal().x - m_pMeshMngr->GetBoundingObject(i)->GetHalfWidth().x;
		}
		else if (max.x < m_pMeshMngr->GetBoundingObject(i)->GetCentroidGlobal().x)
		{
			max.x = m_pMeshMngr->GetBoundingObject(i)->GetCentroidGlobal().x + m_pMeshMngr->GetBoundingObject(i)->GetHalfWidth().x;
		}

		if (min.y > m_pMeshMngr->GetBoundingObject(i)->GetCentroidGlobal().y)
		{
			min.y = m_pMeshMngr->GetBoundingObject(i)->GetCentroidGlobal().y - m_pMeshMngr->GetBoundingObject(i)->GetHalfWidth().y;
		}
		else if (max.y < m_pMeshMngr->GetBoundingObject(i)->GetCentroidGlobal().y)
		{
			max.y = m_pMeshMngr->GetBoundingObject(i)->GetCentroidGlobal().y + m_pMeshMngr->GetBoundingObject(i)->GetHalfWidth().y;
		}

		if (min.z > m_pMeshMngr->GetBoundingObject(i)->GetCentroidGlobal().z)
		{
			min.z = m_pMeshMngr->GetBoundingObject(i)->GetCentroidGlobal().z - m_pMeshMngr->GetBoundingObject(i)->GetHalfWidth().z;
		}
		else if (max.z < m_pMeshMngr->GetBoundingObject(i)->GetCentroidGlobal().z)
		{
			max.z = m_pMeshMngr->GetBoundingObject(i)->GetCentroidGlobal().z + m_pMeshMngr->GetBoundingObject(i)->GetHalfWidth().z;
		}
	}

	vector3 centroid = (min + max) / 2.0f;
	float halfDistance = 0;

	if (halfDistance < glm::distance(vector3(min.x, 0, 0), centroid))
	{
		halfDistance = glm::distance(vector3(min.x, 0, 0), centroid);
	}
	if (halfDistance < glm::distance(vector3(0, min.y, 0), centroid))
	{
		halfDistance = glm::distance(vector3(0, min.y, 0), centroid);
	}
	if (halfDistance < glm::distance(vector3(0, 0, min.z), centroid))
	{
		halfDistance = glm::distance(vector3(0, 0, min.z), centroid);
	}
	if (halfDistance < glm::distance(vector3(max.x, 0, 0), centroid))
	{
		halfDistance = glm::distance(vector3(max.x, 0, 0), centroid);
	}
	if (halfDistance < glm::distance(vector3(0, max.y, 0), centroid))
	{
		halfDistance = glm::distance(vector3(0, max.y, 0), centroid);
	}
	if (halfDistance < glm::distance(vector3(0, 0, max.z), centroid))
	{
		halfDistance = glm::distance(vector3(0, 0, max.z), centroid);
	}

	//OctTree Framework
	m_pMeshMngr->AddAxisToQueue(glm::translate(centroid));
	m_pMeshMngr->AddCubeToQueue(glm::translate(centroid) * glm::scale(vector3(halfDistance * 2.0f)), vector3(1.0f, 1.0f, 1.0f), MERENDER::WIRE);

	vector3 centroidTwo = (centroid + max) / 2.0f;
	m_pMeshMngr->AddAxisToQueue(glm::translate(centroidTwo));
	m_pMeshMngr->AddCubeToQueue(glm::translate(centroidTwo) * glm::scale(vector3(halfDistance)), vector3(1.0f, 1.0f, 1.0f), MERENDER::WIRE);
}


