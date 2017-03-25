#include "TestObject.h"


TestObject::TestObject() : 
	m_State(ONE_STATE),
	m_StateFunc(&TestObject::OneFunc)
{
}

TestObject::~TestObject()
{
}
