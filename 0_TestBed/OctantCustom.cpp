#include "OctantCustom.h"


OctantCustom::OctantCustom(MeshManagerSingleton* m_pMeshMngr, std::vector<BoundingObjectClass*> masterList)
{
	subdivLevel = 0;
	octantID = 0;
	edgeLength = 0.0f;

	CreateOctant(m_pMeshMngr, masterList);
	SubdivideOctant(m_pMeshMngr, masterList);
	RenderOctant(m_pMeshMngr);
}


OctantCustom::OctantCustom(MeshManagerSingleton* m_pMeshMngr, std::vector<BoundingObjectClass*> containedObjects,
						   int i_Level, int i_ID, vector3 centroid, float edge)
{
	subdivLevel = i_Level;
	octantID = i_ID;
	octantCentroid = centroid;
	edgeLength = edge;

	SubdivideOctant(m_pMeshMngr, containedObjects);
	RenderOctant(m_pMeshMngr);
}


OctantCustom::~OctantCustom(void)
{
	// w00t
}


void OctantCustom::CreateOctant(MeshManagerSingleton* m_pMeshMngr, std::vector<BoundingObjectClass*> masterList)
{
	vector3 rootMin;
	vector3 rootMax;

	rootMin = masterList[0]->GetCentroidGlobal() - masterList[0]->GetHalfWidth();
	rootMax = masterList[0]->GetCentroidGlobal() + masterList[0]->GetHalfWidth();

	// find the min/max of the root cube by going through each object and calculating its bounds
	for (BoundingObjectClass* boInstance : masterList)
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
	octantCentroid = (rootMin + rootMax) / 2.0f;

	// calculate that shit
	if (edgeLength < glm::distance(vector3(rootMin.x, 0, 0), vector3(rootMax.x, 0, 0)))
		edgeLength = glm::distance(vector3(rootMin.x, 0, 0), vector3(rootMax.x, 0, 0));
	if (edgeLength < glm::distance(vector3(rootMin.y, 0, 0), vector3(rootMax.y, 0, 0)))
		edgeLength = glm::distance(vector3(rootMin.y, 0, 0), vector3(rootMax.y, 0, 0));
	if (edgeLength < glm::distance(vector3(rootMin.z, 0, 0), vector3(rootMax.z, 0, 0)))
		edgeLength = glm::distance(vector3(rootMin.z, 0, 0), vector3(rootMax.z, 0, 0));
}


void OctantCustom::SubdivideOctant(MeshManagerSingleton* m_pMeshMngr, std::vector<BoundingObjectClass*> containedObjects)
{
	if (subdivLevel < MAX_SUBDIVS)
	{
		vector3 childCentroid;
		float childEdge = edgeLength/4.0f;
		for (int numSubdiv = 0; numSubdiv < 8; numSubdiv++)
		{
			childCentroid = octantCentroid;

			// subdivs are described by how you face them
			switch(numSubdiv)
			{
				case 0:	//	Front Top Right
					childCentroid += vector3(childEdge);
					break;
				case 1:	//	Front Top Left
					childCentroid += vector3(-childEdge, childEdge, childEdge);
					break;
				case 2:	//	Front Bottom Left
					childCentroid += vector3(-childEdge, -childEdge, childEdge);
					break;
				case 3:	//	Front Bottom Right
					childCentroid += vector3(childEdge, -childEdge, childEdge);
					break;
				case 4:	//	Rear Top Right
					childCentroid += vector3(childEdge, childEdge, -childEdge);
					break;
				case 5:	//	Rear Top Left
					childCentroid += vector3(-childEdge, childEdge, -childEdge);
					break;
				case 6:	//	Rear Bottom Left
					childCentroid -= vector3(childEdge);
					break;
				case 7:	//	Rear Bottom Right
					childCentroid += vector3(childEdge, -childEdge, -childEdge);
					break;
			}

			// go through all the objects given and figure out of they're inside of that child 
			for (BoundingObjectClass* conOBJs : containedObjects)
			{
				if (conOBJs->GetCentroidGlobal().x > childCentroid.x - childEdge &&
					conOBJs->GetCentroidGlobal().x < childCentroid.x + childEdge &&
					conOBJs->GetCentroidGlobal().y > childCentroid.y - childEdge &&
					conOBJs->GetCentroidGlobal().y < childCentroid.y + childEdge &&
					conOBJs->GetCentroidGlobal().z > childCentroid.z - childEdge &&
					conOBJs->GetCentroidGlobal().z < childCentroid.z + childEdge)
				{
					// if they are, add them to the internal bounding objects vector
					internalBoundingObjects.push_back(conOBJs);
				}
			}

			// create a new octant if there's more than one object in that child
			if (internalBoundingObjects.size() >= 1)
				childrenOctants.push_back(new OctantCustom(m_pMeshMngr, internalBoundingObjects, subdivLevel+1,
					octantID+(1*numSubdiv), childCentroid, childEdge*2.0f));
		}
	}
}


void OctantCustom::RenderOctant(MeshManagerSingleton* m_pMeshMngr)
{
	vector3 colorVector;

	if (subdivLevel == 0)
		colorVector = vector3(1.0f,1.0f,1.0f);
	else if (subdivLevel == 1)
		colorVector = vector3(1.0f,0.0f,0.0f);
	else if (subdivLevel == 2)
		colorVector = vector3(0.5f,0.5f,1.0f);
	else if (subdivLevel == 3)
		colorVector = vector3(0.5f,0.5f,0.5f);

	if (internalBoundingObjects.size() > 0)
		m_pMeshMngr->AddCubeToQueue(glm::translate(octantCentroid) * glm::scale(vector3(edgeLength)), colorVector, MERENDER::WIRE);
}