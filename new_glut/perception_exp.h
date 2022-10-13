#ifndef __EXP_GEOMETRY_H__
#define __EXP_GEOMETRY_H__

#include <gl/glew.h>
#include <gl/gl.h>
#include <gl/freeglut.h>


class perception_exp
{
private:
	int cnt, tmp;;
public:
	perception_exp();
	~perception_exp();

	void init();
	void onDisplay();
	void mouseClick(GLint, GLint, GLint, GLint);
	void onMultiMotion(int, int, int);
};


#endif	//__EXP_GEOMETRY_H__