#ifndef __OCTANTCUSTOM_H_
#define __OCTANTCUSTOM_H_

#include "ME\MyEngine.h"
using namespace MyEngine;

static const int MAX_SUBDIVS = 3;
static const int MAX_OBJS_PER_OCTANT = 1;

class OctantCustom
{
	public:
		OctantCustom(MeshManagerSingleton*, std::vector<InstanceClass*>);
		OctantCustom(MeshManagerSingleton*, std::vector<InstanceClass*>, int, int, vector3, float);
		~OctantCustom(void);
		void CreateOctant(MeshManagerSingleton*, std::vector<InstanceClass*>);
		void SubdivideOctant(MeshManagerSingleton*, std::vector<InstanceClass*>);
		void RenderOctant(MeshManagerSingleton*);
		String DetectBullet(vector3 bulletCentroid, MeshManagerSingleton*);
		int TestOBBOBB(BoundingObjectClass* a, BoundingObjectClass* b);

		bool isLeaf;		// helps prevent further subdivs from happening
		int subdivLevel;	// level of subdivisions
		int octantID;		// ID of octant
		vector3 octantCentroid;		// center position in world 
		float edgeLength;	// size of the octant's edge, used to calculate volume
		BoundingObjectClass* octBO;
		OctantCustom* parentOctant;				// Parent of this octant; if none, it's the root
		std::vector<OctantCustom*> childrenOctants;		// all 8 children of this octant; if none, it's a leaf
		std::vector<InstanceClass*> internalBoundingObjects;		// contains all objects that are colliding/inside with this octant
};

#endif // __OctantCustom_H_