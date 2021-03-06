﻿#include <gtest/gtest.h>
#include <Altseed.h>
#include "../EngineTest.h"

using namespace std;
using namespace asd;

class ObjectSystem_SceneLifeCycle : public EngineTest
{
	class TestScene : public Scene
	{
	public:
		int m_phase = 0;
		bool m_alreadyDisposed = false;

	protected:
		void OnRegistered()
		{
			ASSERT_EQ(m_phase, 0);
			m_phase = 1;
		}

		void OnStartUpdating()
		{
			ASSERT_EQ(m_phase, 1);
			m_phase = 2;
		}

		void OnTransitionFinished()
		{
			ASSERT_EQ(m_phase, 2);
			m_phase = 3;
		}

		void OnTransitionBegin()
		{
			ASSERT_EQ(m_phase, 3);
			m_phase = 4;
		}

		void OnStopUpdating()
		{
			ASSERT_EQ(m_phase, 4);
			m_phase = 5;
		}

		void OnUnregistered()
		{
			ASSERT_EQ(m_phase, 5);
			m_phase = 6;
		}

		void OnDispose()
		{
			m_alreadyDisposed = true;
		}

	public:
		TestScene()
		{
			m_phase = 0;
		}
	};

public:
	ObjectSystem_SceneLifeCycle(bool isOpenGLMode)
		: EngineTest(asd::ToAString("SceneLifeCycle"), isOpenGLMode, 180)
	{
	}

private:
	int m_count = 0;
	shared_ptr<TestScene> m_scene1, m_scene2, m_scene3;

protected:
	void OnStart()
	{
		m_scene1 = make_shared<TestScene>();
		Engine::ChangeSceneWithTransition(m_scene1, make_shared<TransitionFade>(0.3f, 0.3f));
	}

	void OnUpdating()
	{
		++m_count;
		if (m_count == 60)
		{
			m_scene2 = make_shared<TestScene>();
			Engine::ChangeSceneWithTransition(m_scene2, make_shared<TransitionFade>(0.3f, 0.3f));
		}
		else if (m_count == 120)
		{
			ASSERT_TRUE(m_scene1->m_alreadyDisposed);
			m_scene3 = make_shared<TestScene>();
			Engine::ChangeScene(m_scene3);
		}
		else if (m_count == 122)
		{
			ASSERT_TRUE(m_scene2->m_alreadyDisposed);
			m_scene3->Dispose();
		}
		else if (m_count == 123)
		{
			ASSERT_TRUE(m_scene3->m_alreadyDisposed);
		}
	}

	void OnUpdated()
	{
	}

	void OnFinish()
	{
	}
};

ENGINE_TEST(ObjectSystem, SceneLifeCycle)