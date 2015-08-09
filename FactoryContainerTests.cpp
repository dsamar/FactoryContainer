/*
The MIT License (MIT)

Copyright (c) 2015 Dan Samargiu

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <gtest/gtest.h>
#include "FactoryContainer.hpp"
#include "SampleTestClassHierarchy.h"

namespace
{
using namespace TestClassHierarchy;

TEST(FactoryContainerTest, SimpleRegisterResolvesObject)
{
  FactoryContainer tFactory;
  tFactory.RegisterType<INumber, NumberOne>();

  auto pNumber = tFactory.Resolve<INumber>();
  ASSERT_NE(nullptr, pNumber);
}

TEST(FactoryContainerTest, SimpleRegisterObjectIsCorrectType)
{
  FactoryContainer tFactory;
  tFactory.RegisterType<INumber, NumberTwo>();

  auto pNumber = tFactory.Resolve<INumber>();
  auto iNum = pNumber->What();

  ASSERT_EQ(2, iNum);
}

TEST(FactoryContainerTest, RegisterTwoTypesAndBothTypesAreCorrectlyResolved)
{
  FactoryContainer tFactory;
  tFactory.RegisterType<INumber, NumberOne>();
  tFactory.RegisterType<ISeason, Summer>();

  auto pNumber = tFactory.Resolve<INumber>();
  auto iNum = pNumber->What();

  auto pSeason = tFactory.Resolve<ISeason>();
  auto strWeather = pSeason->Weather();

  ASSERT_EQ(1, iNum);
  ASSERT_EQ(L"Sunny", strWeather);
}

TEST(FactoryContainerTest, TryToResolveWhenNothingRegistered)
{
  FactoryContainer tFactory;
  auto pNumber = tFactory.Resolve<INumber>();
  ASSERT_EQ(nullptr, pNumber);
}

TEST(FactoryContainerTest, AddSameTypeTwiceShouldOverwriteResolveTwo)
{
  FactoryContainer tFactory;
  tFactory.RegisterType<INumber, NumberOne>();
  tFactory.RegisterType<INumber, NumberTwo>();

  auto pNumber = tFactory.Resolve<INumber>();
  auto iNum = pNumber->What();

  ASSERT_EQ(2, iNum);
}

TEST(FactoryContainerTest, AddSameTypeTwiceShouldOverwriteResolveOne)
{
  FactoryContainer tFactory;
  tFactory.RegisterType<INumber, NumberTwo>();
  tFactory.RegisterType<INumber, NumberOne>();

  auto pNumber = tFactory.Resolve<INumber>();
  auto iNum = pNumber->What();

  ASSERT_EQ(1, iNum);
}

TEST(FactoryContainerTest, RegisterInstanceIsSameInstance)
{
  FactoryContainer tFactory;
  auto pNumberExpected = std::make_shared<NumberTwo>();
  tFactory.RegisterInstance<INumber>(pNumberExpected);

  auto pNumberActual = tFactory.Resolve<INumber>();

  ASSERT_EQ(pNumberExpected.get(), pNumberActual.get());

  auto pNumber = tFactory.Resolve<INumber>();
  auto iNum = pNumber->What();

  ASSERT_EQ(2, iNum);
}

TEST(FactoryContainerTest, ConstructorInjectionTest)
{
  FactoryContainer tFactory;
  tFactory.RegisterType<Milk, Milk>();
  tFactory.RegisterType<Cereal, Cereal>();
  tFactory.RegisterType<Breakfast, Breakfast, Milk, Cereal>();

  auto pBreakfast = tFactory.Resolve<Breakfast>();
  ASSERT_NE(nullptr, pBreakfast);

  auto iCalories = pBreakfast->GetCalories();
  ASSERT_EQ(15, iCalories);
}

TEST(FactoryContainerTest, CircularDependencyTest)
{
  FactoryContainer tFactory;
  tFactory.RegisterType<Chicken, Chicken, Egg>();
  tFactory.RegisterType<Egg, Egg, Chicken>();

  auto pChicken = tFactory.Resolve<Chicken>();
  ASSERT_NE(nullptr, pChicken);
  ASSERT_EQ(nullptr, pChicken->GetEgg()->GetChicken());
}

TEST(FactoryContainerTest, CircularDependencyTestMoreComplex)
{
  // A -> B
  // A -> C
  // B -> C
  // C -> B
  FactoryContainer tFactory;
  tFactory.RegisterType<A, A, B, C>();
  tFactory.RegisterType<B, B, C>();
  tFactory.RegisterType<C, C, B>();

  auto pA = tFactory.Resolve<A>();
  ASSERT_NE(nullptr, pA);
  ASSERT_NE(nullptr, pA->GetB());
  ASSERT_NE(nullptr, pA->GetC());
  ASSERT_NE(nullptr, pA->GetB()->GetC());
  ASSERT_NE(nullptr, pA->GetC()->GetB());
  ASSERT_EQ(nullptr, pA->GetB()->GetC()->GetB());
  ASSERT_EQ(nullptr, pA->GetC()->GetB()->GetC());
}
TEST(FactoryContainerTest, CircularDependencyTestEvenMoreComplex)
{
  // Class1 -> Class2 -> Class3 -> Class1
  //                  -> Class1

  FactoryContainer tFactory;
  tFactory.RegisterType<Class1, Class1, Class2>();
  tFactory.RegisterType<Class2, Class2, Class1, Class3>();
  tFactory.RegisterType<Class3, Class3, Class1>();

  auto pC1 = tFactory.Resolve<Class1>();
  ASSERT_NE(nullptr, pC1);
  ASSERT_NE(nullptr, pC1->Get2());
  ASSERT_NE(nullptr, pC1->Get2()->Get3());
  ASSERT_EQ(nullptr, pC1->Get2()->Get1());
  ASSERT_EQ(nullptr, pC1->Get2()->Get3()->Get1());
}
}
