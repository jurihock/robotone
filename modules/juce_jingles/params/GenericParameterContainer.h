#pragma once

#include <juce_jingles/params/GenericParameterSubscription.h>

BEGIN_JUCE_JINGLES_NAMESPACE

class GenericParameterContainer
{

public:

  using AnyRangedAudioParameter = std::variant<
    juce::AudioParameterBool*,
    juce::AudioParameterInt*,
    juce::AudioParameterFloat*,
    juce::AudioParameterChoice*>;

  using GenericParameterNotification = std::function<void(
    AnyRangedAudioParameter)>;

  GenericParameterContainer(juce::AudioProcessor& process) :
    process(process)
  {
  }

  juce::RangedAudioParameter* get(const std::string& id) const
  {
    return std::visit(
      [](juce::RangedAudioParameter* parameter)
      { return parameter; },
      parameters.at(id));
  }

  template<typename T>
  T get(const std::string& id) const
  {
    static_assert(missing_template_specialization<T>::value);
  }

  template<typename T>
  void set(const std::string& id, const T value) const
  {
    static_assert(missing_template_specialization<T>::value);
  }

  void notify(const std::string& ns, std::function<void()> callback)
  {
    notifications[ns].emplace_back([callback](AnyRangedAudioParameter)
    {
      callback();
    });
  }

  void notify(const std::string& ns, std::function<void(
    const std::string& id,
    const std::variant<bool, int, double, std::string> value)> callback)
  {
    notifications[ns].emplace_back([callback](AnyRangedAudioParameter parameter)
    {
      auto id = std::visit([](juce::RangedAudioParameter* pointer) {
        return pointer->getParameterID().toStdString();
      }, parameter);

      std::visit(visitor{
        [&](juce::AudioParameterBool*   pointer) { callback(id, *pointer); },
        [&](juce::AudioParameterInt*    pointer) { callback(id, *pointer); },
        [&](juce::AudioParameterFloat*  pointer) { callback(id, static_cast<double>(*pointer)); },
        [&](juce::AudioParameterChoice* pointer) { callback(id, pointer->getCurrentChoiceName().toStdString()); }
      }, parameter);
    });
  }

protected:

  template<typename T>
  struct missing_template_specialization : std::false_type {};

  template<class... T>
  struct visitor : T... { using T::operator()...; };

  // explicit deduction guide,
  // which is actually not needed as of C++ 20,
  // but still required for Apple Clang 15
  template<class... Ts>
  visitor(Ts...) -> visitor<Ts...>;

  void add(const std::string& ns, AnyRangedAudioParameter parameter)
  {
    std::visit([ns, parameter, this](juce::RangedAudioParameter* pointer)
    {
      auto id = pointer->getParameterID().toStdString();

      auto callback = [ns, parameter, this]()
      {
        const auto& callbacks = notifications.find(ns);

        if (callbacks == notifications.end())
        {
          return;
        }

        for (const auto& callback : callbacks->second)
        {
          callback(parameter);
        }
      };

      auto subscription = std::make_unique<GenericParameterSubscription>(pointer, callback);

      parameters.emplace(id, parameter);
      subscriptions.push_back(std::move(subscription));
      process.addParameter(pointer);
    }, parameter);
  }

  void visit(std::function<void(const std::string& id, const AnyRangedAudioParameter& parameter)> callback)
  {
    for (const auto& [id, parameter] : parameters)
    {
      callback(id, parameter);
    }
  }

private:

  juce::AudioProcessor& process;

  std::map<std::string, AnyRangedAudioParameter> parameters;
  std::vector<std::unique_ptr<GenericParameterSubscription>> subscriptions;
  std::map<std::string, std::vector<GenericParameterNotification>> notifications;

};

template<>
inline bool GenericParameterContainer::get<bool>(const std::string& id) const
{
  auto* parameter = std::get<juce::AudioParameterBool*>(parameters.at(id));

  return *parameter;
}

template<>
inline int GenericParameterContainer::get<int>(const std::string& id) const
{
  auto* parameter = std::get<juce::AudioParameterInt*>(parameters.at(id));

  return *parameter;
}

template<>
inline float GenericParameterContainer::get<float>(const std::string& id) const
{
  auto* parameter = std::get<juce::AudioParameterFloat*>(parameters.at(id));

  return *parameter;
}

template<>
inline double GenericParameterContainer::get<double>(const std::string& id) const
{
  auto* parameter = std::get<juce::AudioParameterFloat*>(parameters.at(id));

  return static_cast<double>(*parameter);
}

template<>
inline std::string GenericParameterContainer::get<std::string>(const std::string& id) const
{
  auto* parameter = std::get<juce::AudioParameterChoice*>(parameters.at(id));

  return parameter->getCurrentChoiceName().toStdString();
}

template<>
inline void GenericParameterContainer::set<bool>(const std::string& id, const bool value) const
{
  auto* parameter = std::get<juce::AudioParameterBool*>(parameters.at(id));

  *parameter = value;
}

template<>
inline void GenericParameterContainer::set<int>(const std::string& id, const int value) const
{
  auto* parameter = std::get<juce::AudioParameterInt*>(parameters.at(id));

  *parameter = value;
}

template<>
inline void GenericParameterContainer::set<float>(const std::string& id, const float value) const
{
  auto* parameter = std::get<juce::AudioParameterFloat*>(parameters.at(id));

  *parameter = value;
}

template<>
inline void GenericParameterContainer::set<double>(const std::string& id, const double value) const
{
  auto* parameter = std::get<juce::AudioParameterFloat*>(parameters.at(id));

  *parameter = static_cast<float>(value);
}

template<>
inline void GenericParameterContainer::set<std::string>(const std::string& id, const std::string value) const
{
  auto* parameter = std::get<juce::AudioParameterChoice*>(parameters.at(id));

  const juce::StringArray& choices = parameter->choices;
  const int index = choices.indexOf(std::move(value), true);

  if (index >= 0)
  {
    *parameter = index;
  }
}

END_JUCE_JINGLES_NAMESPACE
