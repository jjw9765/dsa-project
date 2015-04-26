/*--------------------------------------------------------------------------------------------------
Created by Alberto Bobadilla (labigm@rit.edu) in 2014
--------------------------------------------------------------------------------------------------*/
#ifndef __FRAMECLASS_H_
#define __FRAMECLASS_H_

#include "ME\system\SystemSingleton.h"

namespace MyEngine
{

class MyEngineDLL FrameClass
{
	bool m_bKeyframe;
	bool m_bVisible;
	vector3 m_v3Translation;
	vector3 m_v3Rotation;
	vector3 m_v3Scale;
	matrix4 m_m4Transformation;
public:
	
	//Constructor
	FrameClass();
	//Copy Constructor
	FrameClass(const FrameClass& other);
	//Copy Assignment Operator
	virtual FrameClass& operator=(const FrameClass& other);

	//Destructor
	~FrameClass(void);

	matrix4 GetTransformation(void);
	matrix4 ComputeTransformation(	vector3 a_v3Translation,
									vector3 a_v3Pivot,
									vector3 a_v3Rotation,
									vector3 a_v3Scale,
									bool a_bVisible,
									bool a_bKeyFrame,
									matrix4 a_m4ToParent = matrix4(1.0f));
	matrix4 ComputeTransformation(matrix4 a_m4ToParent);
	matrix4 ComputeTransformation(vector3 a_v3Pivot);
	
	bool IsKeyframe(void);
	void SetVisible(bool a_bVisible);
	bool IsVisible(void);
	void SetKeyframe(bool a_bKeyframe);

	void SetPosX( float a_fValue );
	void SetPosY( float a_fValue );
	void SetPosZ( float a_fValue );

	void SetAngX( float a_fValue );
	void SetAngY( float a_fValue );
	void SetAngZ( float a_fValue );

	void SetSizeX( float a_fValue );
	void SetSizeY( float a_fValue );
	void SetSizeZ( float a_fValue );

	float GetPosX( void );
	float GetPosY( void );
	float GetPosZ( void );

	float GetAngX( void );
	float GetAngY( void );
	float GetAngZ( void );

	float GetSizeX( void );
	float GetSizeY( void );
	float GetSizeZ( void );
};

EXPIMP_TEMPLATE template class MyEngineDLL std::vector<FrameClass>;
EXPIMP_TEMPLATE template class MyEngineDLL std::vector<FrameClass*>;

}
#endif //__ANIMATIONFRAMECLASS_H_