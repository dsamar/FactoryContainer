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
#include <unordered_map>
#include <memory>
#include <functional>
#include <typeindex>
#include <type_traits>
#include <vector>
#include <algorithm>

class FactoryContainer
{
private:
  typedef std::vector<std::type_index> ancestor_list_type;
  typedef std::function<std::shared_ptr<void>(ancestor_list_type*)> factory_value;

public:
  FactoryContainer() : m_factoryList()
  {
  }

  // Register Type I with Concrete Implementation T and dependencies
  // expanded from rest of template arguments.
  // Calls to Resolve will instantiate type T and recursively resolve
  // dependencies passing them into T's constructor as shared_ptrs
  // Internally calls Unregister on previous registrations for type I
  template <typename I, typename T, typename... Arguments>
  void RegisterType()
  {
    // Check type traits between I and T
    // I must be the base of T
    if(!std::is_base_of<I,T>())
    {
      return;
    }

    auto tFactory = [this](ancestor_list_type * ancestor_list)
    {
      // sometimes we don't need the ancestor list as the argument package
      // is empty. this avoids unused parameter warning
      (void) ancestor_list;
      return std::make_shared<T>(this->Resolve<Arguments>(ancestor_list)...);
    };
    RegisterFactory<I>(std::move(tFactory));
  }

  // Register Type I with an instance defined by pInstance.
  // Calls to resolve will return the pointer to pInstance.
  // Internally calls Unregister on previous registrations for type I
  template <typename I>
  void RegisterInstance(std::shared_ptr<I> pInstance)
  {
    auto tFactory = [pInstance] (ancestor_list_type*)
    {
      return pInstance;
    };
    RegisterFactory<I>(std::move(tFactory));
  }

  // Remove the associated type from the factory lookup
  // Calls to Resolve<I> will return nullptr after this call
  template <typename I>
  void Unregister()
  {
    const auto key = std::type_index(typeid(I));
    const auto it = m_factoryList.find(key);
    if (it != m_factoryList.end())
    {
      m_factoryList.erase(it);
    }
  }

  // Resolve the registered type I
  // Returns a shared_ptr to object based on type or instance registrations
  template <typename I>
  std::shared_ptr<I> Resolve() const
  {
    // ancestor list for dependency loop detection
    ancestor_list_type ancestor_list;
    return Resolve<I>(&ancestor_list);
  }

private:
  std::unordered_map<std::type_index, factory_value> m_factoryList;

  template <typename I>
  void RegisterFactory(factory_value&& tFactory)
  {
    // Always unregister so we don't end up with duplicate entries.
    Unregister<I>();

    // Push the factory onto our list.
    const auto key = std::type_index(typeid(I));
    m_factoryList[key] = std::move(tFactory);
  }

  template <typename I>
  std::shared_ptr<I> Resolve(ancestor_list_type* ancestor_list) const
  {
    const auto key = std::type_index(typeid(I));
    const auto it = m_factoryList.find(key);
    if (it == m_factoryList.end())
    {
      return nullptr;
    }

    // If this type is already in the ancestor list, return nullptr
    // to prevent circular dependency loop
    auto found = std::find(ancestor_list->begin(), ancestor_list->end(), key);
    if ( found != ancestor_list->end())
    {
      return nullptr;
    }

    // Depth first traversal, push key onto the list
    // so we can detect circular dependencies
    ancestor_list->push_back(key);
    const auto pObj = it->second(ancestor_list);
    ancestor_list->pop_back();

    return std::static_pointer_cast<I>(pObj);
  }

  // Disable Copy and Assign.
  FactoryContainer(FactoryContainer&) = delete;
  void operator=(FactoryContainer) = delete;
};
