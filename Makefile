test: FactoryContainerTests.cpp FactoryContainer.hpp SampleTestClassHierarchy.h
		g++ FactoryContainerTests.cpp -lgtest -lgtest_main -lpthread -std=c++11 -O2
