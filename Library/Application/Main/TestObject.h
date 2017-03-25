#include <stdio.h>

class TestObject
{
public:
	enum STATE
	{
		ONE_STATE,
		TWO_STATE,
		THREE_STATE
	};
	
	TestObject();
	~TestObject();

	void Update() { (this->*m_StateFunc)(); };
	void SetState(STATE _state)
	{ 
		m_State = _state; 

		switch (m_State)
		{
		case ONE_STATE:
			m_StateFunc = &TestObject::OneFunc;
			break;
		case TWO_STATE:
			m_StateFunc = &TestObject::TwoFunc;
			break;
		case THREE_STATE:
			m_StateFunc = &TestObject::ThreeFunc;
			break;
		}

	}

private:
	STATE m_State;
	void(TestObject::*m_StateFunc)();
	void OneFunc(){ printf("One\n"); }
	void TwoFunc(){ printf("Two\n"); }
	void ThreeFunc(){ printf("Three\n"); }

};

