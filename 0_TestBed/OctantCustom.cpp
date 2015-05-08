#include "OctantCustom.h"

OctantCustom::OctantCustom(MeshManagerSingleton* m_pMeshMngr, std::vector<InstanceClass*> masterList)
{
	isLeaf = false;
	subdivLevel = 0;
	octantID = 0;
	edgeLength = 0.0f;
	
	CreateOctant(m_pMeshMngr, masterList);
		octBO = new BoundingObjectClass(octantCentroid, edgeLength);
		octBO->SetVisibleAABB(false);
	SubdivideOctant(m_pMeshMngr, masterList);
	RenderOctant(m_pMeshMngr);
}


OctantCustom::OctantCustom(MeshManagerSingleton* m_pMeshMngr, std::vector<InstanceClass*> containedObjects,
						   int i_Level, int i_ID, vector3 centroid, float edge)
{
	isLeaf = false;
	subdivLevel = i_Level;
	octantID = i_ID;
	octantCentroid = centroid;
	edgeLength = edge;

		octBO = new BoundingObjectClass(octantCentroid, edgeLength);
		octBO->SetVisibleAABB(false);
	SubdivideOctant(m_pMeshMngr, containedObjects);
	RenderOctant(m_pMeshMngr);
}


OctantCustom::~OctantCustom(void)
{
	// w00t
}


void OctantCustom::CreateOctant(MeshManagerSingleton* m_pMeshMngr, std::vector<InstanceClass*> masterList)
{
	if (masterList.size() > 0)
	{
		vector3 rootMin;
		vector3 rootMax;

		rootMin = masterList[0]->GetGrandBoundingObject()->GetCentroidGlobal() - masterList[0]->GetGrandBoundingObject()->GetHalfWidth();
		rootMax = masterList[0]->GetGrandBoundingObject()->GetCentroidGlobal() + masterList[0]->GetGrandBoundingObject()->GetHalfWidth();

		// find the min/max of the root cube by going through each object and calculating its bounds
		for (InstanceClass* boInstance : masterList)
		{
			if (rootMin.x > boInstance->GetGrandBoundingObject()->GetCentroidGlobal().x - boInstance->GetGrandBoundingObject()->GetHalfWidth().x)
				rootMin.x = boInstance->GetGrandBoundingObject()->GetCentroidGlobal().x - boInstance->GetGrandBoundingObject()->GetHalfWidth().x;
			else if (rootMax.x < boInstance->GetGrandBoundingObject()->GetCentroidGlobal().x + boInstance->GetGrandBoundingObject()->GetHalfWidth().x)
				rootMax.x = boInstance->GetGrandBoundingObject()->GetCentroidGlobal().x + boInstance->GetGrandBoundingObject()->GetHalfWidth().x;

			if (rootMin.y > boInstance->GetGrandBoundingObject()->GetCentroidGlobal().y - boInstance->GetGrandBoundingObject()->GetHalfWidth().y)
				rootMin.y = boInstance->GetGrandBoundingObject()->GetCentroidGlobal().y - boInstance->GetGrandBoundingObject()->GetHalfWidth().y;
			else if (rootMax.y < boInstance->GetGrandBoundingObject()->GetCentroidGlobal().y + boInstance->GetGrandBoundingObject()->GetHalfWidth().y)
				rootMax.y = boInstance->GetGrandBoundingObject()->GetCentroidGlobal().y + boInstance->GetGrandBoundingObject()->GetHalfWidth().y;

			if (rootMin.z > boInstance->GetGrandBoundingObject()->GetCentroidGlobal().z - boInstance->GetGrandBoundingObject()->GetHalfWidth().z)
				rootMin.z = boInstance->GetGrandBoundingObject()->GetCentroidGlobal().z - boInstance->GetGrandBoundingObject()->GetHalfWidth().z;
			else if (rootMax.z < boInstance->GetGrandBoundingObject()->GetCentroidGlobal().z + boInstance->GetGrandBoundingObject()->GetHalfWidth().z)
				rootMax.z = boInstance->GetGrandBoundingObject()->GetCentroidGlobal().z + boInstance->GetGrandBoundingObject()->GetHalfWidth().z;
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
}


// check colliding objects first, then either leaf (leave) it as is, or subdivide again
void OctantCustom::SubdivideOctant(MeshManagerSingleton* m_pMeshMngr, std::vector<InstanceClass*> containedObjects)
{
	vector3 childCentroid;
	float childEdge = edgeLength/4.0f;

	// go through all the objects given and figure out of they're colliding with that child 
	for (InstanceClass* conOBJs : containedObjects)
	{
		if (conOBJs->GetGrandBoundingObject()->IsColliding(*octBO))
		{
			// if they are, add them to the internal bounding objects
			internalBoundingObjects.push_back(conOBJs);
		}
	}

	// if there's only one object in that octant, leaf it, and prevent it from subdividing
	if (internalBoundingObjects.size() == MAX_OBJS_PER_OCTANT)
	{
		isLeaf = true;
	}
	// create a new octant if there's more than one object in that child
	else if (internalBoundingObjects.size() > MAX_OBJS_PER_OCTANT)
	{
		if (subdivLevel < MAX_SUBDIVS && isLeaf == false)
		{
			vector3 childCentroid;
			float childEdge = edgeLength/4.0f;

			// loop 8 times, once for each oct-child
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

				// create a new child octant
				childrenOctants.push_back(new OctantCustom(m_pMeshMngr, internalBoundingObjects, subdivLevel+1,
					octantID+(numSubdiv), childCentroid, childEdge*2.0f));
			}
		}
	}
}


// simply renders the octants that have objects inside of them
void OctantCustom::RenderOctant(MeshManagerSingleton* m_pMeshMngr)
{
	vector3 colorVector;

	// helps colorcode the octant tiers
	if (subdivLevel == 0)
		colorVector = vector3(1.0f,1.0f,1.0f);
	else if (subdivLevel == 1)
		colorVector = vector3(1.0f,0.0f,0.0f);
	else if (subdivLevel == 2)
		colorVector = vector3(0.5f,0.5f,1.0f);
	else if (subdivLevel == 3)

		colorVector = vector3(0.5f,0.5f,0.5f);

	
	if (internalBoundingObjects.size() > 0 )
	{
		//octBO->SetVisibleAABB(true);
		//octBO->Render(false);
	
		m_pMeshMngr->AddCubeToQueue(glm::translate(octantCentroid) * glm::scale(vector3(edgeLength)), colorVector, MERENDER::WIRE);
	}
}


String OctantCustom::DetectBullet(vector3 bulletCentroid, MeshManagerSingleton* m_pMeshMngr)
{
	String objectName;
	if (internalBoundingObjects.size() > 1)
	{
		for (OctantCustom* child : childrenOctants)
		{
			objectName = child->DetectBullet(bulletCentroid, m_pMeshMngr);
			if (objectName.compare("lol") != 0)
				return objectName;
		}
	}
	else
	{
		BoundingObjectClass* bullet = new BoundingObjectClass(bulletCentroid, 1);
		//bullet->SetVisibleOBB(true);
		//bullet->Render(false);

		if (bullet->IsColliding(*octBO))
		{
			m_pMeshMngr->AddCubeToQueue(glm::translate(octantCentroid) * glm::scale(vector3(edgeLength)), vector3(0.0f,1.0f,0.0f), MERENDER::WIRE);

			//if(TestOBBOBB(bullet, intOBJ) == 1)
			for (InstanceClass* conOBJs : internalBoundingObjects)
				if(bullet->IsColliding(*conOBJs->GetGrandBoundingObject()))
				{
					m_pMeshMngr->AddCubeToQueue(glm::translate(conOBJs->GetGrandBoundingObject()->GetCentroidGlobal()) * glm::scale(vector3(1.0f)), vector3(0.0f,1.0f,0.0f), MERENDER::SOLID);
					return conOBJs->GetName();
				}
		}
	}

	return "lol";
}


int OctantCustom::TestOBBOBB(BoundingObjectClass* a, BoundingObjectClass* b)
{
	vector4 u1[3];
	vector4 u2[3];

	//Vector u[3]; Local x-, y-, and z-axes
	u1[0] = vector4(a->GetModelMatrix() * vector4(1.0, 0.0, 0.0, 0.0));
	u1[1] = vector4(a->GetModelMatrix() * vector4(0.0, 1.0, 0.0, 0.0));
	u1[2] = vector4(a->GetModelMatrix() * vector4(0.0, 0.0, 1.0, 0.0));

	//Vector u[3]; Local x-, y-, and z-axes
	u2[0] = vector4(b->GetModelMatrix() * vector4(1.0, 0.0, 0.0, 0.0));
	u2[1] = vector4(b->GetModelMatrix() * vector4(0.0, 1.0, 0.0, 0.0));
	u2[2] = vector4(b->GetModelMatrix() * vector4(0.0, 0.0, 1.0, 0.0));

	//Vector e; Positive halfwidth extents of OBB along each axis
	vector3 eA = a->GetHalfWidth();
	vector3 eB = b->GetHalfWidth();

	float ra, rb;
    glm::mat3x3 R, AbsR;

    // Compute rotation matrix expressing b in a's coordinate frame
    for (int i = 0; i < 3; i++)
       for (int j = 0; j < 3; j++)
		   R[i][j] = glm::dot(u1[i], u2[j]);

    // Compute translation vector t
    vector4 t = b->GetModelMatrix() * vector4(b->GetCentroidGlobal(), 1.0f) - a->GetModelMatrix() * vector4(a->GetCentroidGlobal(), 1.0f);
    // Bring translation into a's coordinate frame
    t = vector4(glm::dot(t, u1[0]), glm::dot(t, u1[1]), glm::dot(t, u1[2]), 1.0f);

    // Compute common subexpressions. Add in an epsilon term to
    // counteract arithmetic errors when two edges are parallel and
    // their cross product is (near) null (see text for details)
    for (int i = 0; i < 3; i++)
       for (int j = 0; j < 3; j++)
           AbsR[i][j] = glm::abs(R[i][j]) + FLT_EPSILON;

    // Test axes L = A0, L = A1, L = A2
    for (int i = 0; i < 3; i++) {
        ra = eA[i];
        rb = eB[0] * AbsR[i][0] + eB[1] * AbsR[i][1] + eB[2] * AbsR[i][2];
        if (glm::abs(t[i]) > ra + rb) return 0;
    }

    // Test axes L = B0, L = B1, L = B2
    for (int i = 0; i < 3; i++) {
        ra = eA[0] * AbsR[0][i] + eA[1] * AbsR[1][i] + eA[2] * AbsR[2][i];
        rb = eB[i];
        if (glm::abs(t[0] * R[0][i] + t[1] * R[1][i] + t[2] * R[2][i]) > ra + rb) return 0;
    }

    // Test axis L = A0 x B0
    ra = eA[1] * AbsR[2][0] + eA[2] * AbsR[1][0];
    rb = eB[1] * AbsR[0][2] + eB[2] * AbsR[0][1];
    if (glm::abs(t[2] * R[1][0] - t[1] * R[2][0]) > ra + rb) return 0;

    // Test axis L = A0 x B1
    ra = eA[1] * AbsR[2][1] + eA[2] * AbsR[1][1];
    rb = eB[0] * AbsR[0][2] + eB[2] * AbsR[0][0];
    if (glm::abs(t[2] * R[1][1] - t[1] * R[2][1]) > ra + rb) return 0;

    // Test axis L = A0 x B2
    ra = eA[1] * AbsR[2][2] + eA[2] * AbsR[1][2];
    rb = eB[0] * AbsR[0][1] + eB[1] * AbsR[0][0];
    if (glm::abs(t[2] * R[1][2] - t[1] * R[2][2]) > ra + rb) return 0;

    // Test axis L = A1 x B0
    ra = eA[0] * AbsR[2][0] + eA[2] * AbsR[0][0];
    rb = eB[1] * AbsR[1][2] + eB[2] * AbsR[1][1];

    if (glm::abs(t[0] * R[2][0] - t[2] * R[0][0]) > ra + rb) return 0;

    // Test axis L = A1 x B1
    ra = eA[0] * AbsR[2][1] + eA[2] * AbsR[0][1];
    rb = eB[0] * AbsR[1][2] + eB[2] * AbsR[1][0];
    if (glm::abs(t[0] * R[2][1] - t[2] * R[0][1]) > ra + rb) return 0;

    // Test axis L = A1 x B2
    ra = eA[0] * AbsR[2][2] + eA[2] * AbsR[0][2];
    rb = eB[0] * AbsR[1][1] + eB[1] * AbsR[1][0];
    if (glm::abs(t[0] * R[2][2] - t[2] * R[0][2]) > ra + rb) return 0;

    // Test axis L = A2 x B0
    ra = eA[0] * AbsR[1][0] + eA[1] * AbsR[0][0];
    rb = eB[1] * AbsR[2][2] + eB[2] * AbsR[2][1];
    if (glm::abs(t[1] * R[0][0] - t[0] * R[1][0]) > ra + rb) return 0;

    // Test axis L = A2 x B1
    ra = eA[0] * AbsR[1][1] + eA[1] * AbsR[0][1];
    rb = eB[0] * AbsR[2][2] + eB[2] * AbsR[2][0];
    if (glm::abs(t[1] * R[0][1] - t[0] * R[1][1]) > ra + rb) return 0;

    // Test axis L = A2 x B2
    ra = eA[0] * AbsR[1][2] + eA[1] * AbsR[0][2];
    rb = eB[0] * AbsR[2][1] + eB[1] * AbsR[2][0];
    if (glm::abs(t[1] * R[0][2] - t[0] * R[1][2]) > ra + rb) return 0;

    // Since no separating axis is found, the OBBs must be intersecting
    return 1;
}