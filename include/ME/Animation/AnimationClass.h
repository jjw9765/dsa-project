/*
Programmer: Alberto Bobadilla (labigm@gmail.com)
Date: 2014/05
*/
#ifndef __ANIMATIONCLASS_H_
#define __ANIMATIONCLASS_H_

#include "ME\System\SystemSingleton.h"
#include "ME\Animation\FrameClass.h"

namespace MyEngine
{

class MyEngineDLL AnimationClass
{
public:
	AnimationClass(void);
	AnimationClass(const AnimationClass& other);
	AnimationClass& operator=(const AnimationClass& other);
	~AnimationClass(void);
	void Init(void);
	void Release(void);
	
	void Swap(AnimationClass& other);

	AnimationClass(FrameClass *a_pFrame, int a_nFrames);
	void CheckInterpolation(FrameClass *a_pFrame, int a_nFrames) const;
private:
	void Interpolation(FrameClass *a_pFrame, int a_nFirstKeyFrame, int a_nLastKeyFrame) const;
};

EXPIMP_TEMPLATE template class MyEngineDLL std::vector<AnimationClass>;
EXPIMP_TEMPLATE template class MyEngineDLL std::vector<AnimationClass*>;

}
#endif //__ANIMATIONCLASS_H__