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

#pragma once
#include <memory>

namespace TestClassHierarchy
{
// Numbers

class INumber
{
public:
  virtual ~INumber() {};
  virtual int What() = 0;
};

class NumberOne : public INumber
{
public:
  virtual int What()
  {
    return 1;
  }
};

class NumberTwo : public INumber
{
public:
  virtual int What()
  {
    return 2;
  }
};

// Seasons

class ISeason
{
public:
  virtual ~ISeason() {};
  virtual std::wstring Weather() = 0;
};

class Summer : public ISeason
{
public:
  virtual std::wstring Weather()
  {
    return L"Sunny";
  }
};

class Winter : public ISeason
{
public:
  virtual std::wstring Weather()
  {
    return L"Snowy";
  }
};

// Constructor Injection

class Milk
{
public:
  int GetCalories()
  {
    return 10;
  }
};

class Cereal
{
public:
  int GetCalories()
  {
    return 5;
  }
};

class Breakfast
{
public:
  Breakfast(std::shared_ptr<Milk> pMilk, std::shared_ptr<Cereal> pCereal) : m_pMilk(pMilk), m_pCereal(pCereal)
  {

  }

  int GetCalories()
  {
    return m_pCereal->GetCalories() + m_pMilk->GetCalories();
  }

private:
  std::shared_ptr<Milk> m_pMilk;
  std::shared_ptr<Cereal> m_pCereal;
};

class Egg;

class Chicken
{
public:
  Chicken(std::shared_ptr<Egg> pEgg) : m_pEgg(pEgg)
  {

  }

  Egg* GetEgg()
  {
    return m_pEgg.get();
  }
private:
  std::shared_ptr<Egg> m_pEgg;
};

class Egg
{
public:
  Egg(std::shared_ptr<Chicken> pChicken) : m_pChicken(pChicken)
  {

  }

  Chicken* GetChicken()
  {
    return m_pChicken.get();
  }
private:
  std::shared_ptr<Chicken> m_pChicken;
};

// A -> B
// A -> C
// B -> C
// C -> B
class B;
class C;
class A
{
public:
  A(std::shared_ptr<B> pB, std::shared_ptr<C> pC) : m_pB(pB), m_pC(pC) {}
  B* GetB() { return m_pB.get(); }
  C* GetC() { return m_pC.get(); }
private:
  std::shared_ptr<B> m_pB;
  std::shared_ptr<C> m_pC;
};

class B
{
public:
  B(std::shared_ptr<C> pC) : m_pC(pC) {}
  C* GetC() { return m_pC.get(); }
private:
  std::shared_ptr<C> m_pC;
};

class C
{
public:
  C(std::shared_ptr<B> pB) : m_pB(pB) {}
  B* GetB() { return m_pB.get(); }
private:
  std::shared_ptr<B> m_pB;
};

// Class1 -> Class2 -> Class3 -> Class1
//                  -> Class1

class Class2;
class Class1
{
public:
  Class1(std::shared_ptr<Class2> p2) : m_p2(p2) {}
  Class2* Get2() { return m_p2.get(); }
private:
  std::shared_ptr<Class2> m_p2;
};

class Class3;
class Class2
{
public:
  Class2(std::shared_ptr<Class1> p1,
         std::shared_ptr<Class3> p3) : m_p1(p1), m_p3(p3) {}
  Class1* Get1() { return m_p1.get(); }
  Class3* Get3() { return m_p3.get(); }
private:
  std::shared_ptr<Class1> m_p1;
  std::shared_ptr<Class3> m_p3;
};

class Class3
{
public:
  Class3(std::shared_ptr<Class1> p1) : m_p1(p1) {}
  Class1* Get1() { return m_p1.get(); }
private:
  std::shared_ptr<Class1> m_p1;
};
}
