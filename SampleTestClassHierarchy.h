/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
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

// Some test cases for unrelated classes.
class UnrelatedBase
{
public:
  UnrelatedBase() {}
};

class UnrelatedDerived
{
public:
  UnrelatedDerived() {}
};
}
