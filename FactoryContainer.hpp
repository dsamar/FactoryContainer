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

#include <map>
#include <memory>
#include <functional>
#include <typeindex>

namespace NUtility
{
  class FactoryContainer
  {
  private:
    typedef std::wstring str;
    const wchar_t* kNoName = L"NO_NAME";

  public:
    typedef std::function<std::shared_ptr<void>()> TFactory;

    FactoryContainer() : m_factoryList()
    {
    }

    template <typename I>
    void RegisterFactory(const str& strNamedRegistration, TFactory&& tFactory)
    {
      // Always unregister so we don't end up with duplicate entries.
      Unregister<I>(strNamedRegistration);

      // Push the factory onto our list.
      const auto tIndex = std::make_pair(std::type_index(typeid(I)), strNamedRegistration);
      m_factoryList[tIndex] = std::move(tFactory);
    }

    template <typename I, typename T, typename... Arguments>
    void RegisterType(const str& strNamedRegistration)
    {
      auto tFactory = [&] { return std::make_shared<T>(this->Resolve<Arguments>()...); };
      RegisterFactory<I>(strNamedRegistration, std::move(tFactory));
    }

    template <typename I>
    void RegisterInstance(const str& strNamedRegistration, std::shared_ptr<I> pInstance)
    {
      auto tFactory = [=] { return pInstance; };
      RegisterFactory<I>(strNamedRegistration, std::move(tFactory));
    }

    template <typename I>
    void RegisterFactory(TFactory&& tFactory)
    {
      RegisterFactory<I>(kNoName);
    }

    template <typename I, typename T, typename... Arguments>
    void RegisterType()
    {
      RegisterType<I, T, Arguments...>(kNoName);
    }

    template <typename I>
    void RegisterInstance(std::shared_ptr<I> pInstance)
    {
      RegisterInstance<I>(kNoName, pInstance);
    }

    template <typename I>
    void Unregister(const str& strNamedRegistration)
    {
      const auto tPair = std::make_pair(std::type_index(typeid(I)), strNamedRegistration);
      const auto it = m_factoryList.find(tPair);
      if (it != m_factoryList.end())
      {
        m_factoryList.erase(it);
      }
    }

    template <typename I>
    void Unregister()
    {
      Unregister<I>(kNoName);
    }

    template <typename I>
    std::shared_ptr<I> Resolve(const str& strNamedRegistration) const
    {
      const auto tPair = std::make_pair(std::type_index(typeid(I)), strNamedRegistration);
      const auto it = m_factoryList.find(tPair);
      if (it != m_factoryList.end())
      {
        const auto pObj = it->second();
        return std::static_pointer_cast<I>(pObj);
      }

      return nullptr;
    }

    template <typename I>
    std::shared_ptr<I> Resolve() const
    {
      return Resolve<I>(kNoName);
    }

  private:
    typedef std::pair<std::type_index, str> TIndex;
    std::map<TIndex, TFactory> m_factoryList;

    // Disable Copy and Assign.
    FactoryContainer(FactoryContainer&) = delete;
    void operator=(FactoryContainer) = delete;
  };
}
