#pragma once

#include <GL/glew.h>

#include <functional>
#include <string>
#include <vector>
#include <utility>
#include <iostream>

namespace test
{
	class Test
	{
	public:
		Test() {}
		virtual ~Test() {}

		virtual void OnUpdate(float deltaTime) {}
		virtual void OnRender() { glClearColor(0.3f, 0.4f, 0.0f, 1.0f); }
		virtual void OnImGuiRender() {}
	};

	class TestMenu : public Test
	{
	public:
		TestMenu(Test *&currentTestPointer);
		~TestMenu() = default;

		void OnImGuiRender() override;

		template<typename T>
		void RegisterTest(const std::string &name)
		{
			std::cout << "Registering test: " << name << std::endl;
			m_Tests.push_back(std::make_pair(name, []() { return new T(); }));
		}
	private:
		Test *&m_CurrentTest;
		std::vector<std::pair<std::string, std::function<Test*()>>> m_Tests;
	};
}

