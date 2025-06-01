#pragma once

#include <juce_jingles/params/GenericParameterContainer.h>

BEGIN_JUCE_JINGLES_NAMESPACE

class XmlParameters : public GenericParameterContainer
{

public:

  XmlParameters(juce::AudioProcessor& process, int schema, std::string tag) :
    GenericParameterContainer(process),
    schema(schema),
    tag(std::move(tag))
  {
  }

  void load(const void* data, const int size,
            std::optional<std::function<void(const std::string& xml)>> callback = std::nullopt);

  void save(juce::MemoryBlock& data,
            std::optional<std::function<void(const std::string& xml)>> callback = std::nullopt);

protected:

  const int schema;
  const std::string tag;

private:

  template<typename T>
  void read(const std::string& id, juce::XmlElement& parent) const
  {
    static_assert(missing_template_specialization<T>::value);
  }

  template<typename T>
  void write(const std::string& id, juce::XmlElement& parent) const
  {
    static_assert(missing_template_specialization<T>::value);
  }

};

template<>
inline void XmlParameters::read<bool>(const std::string& id, juce::XmlElement& parent) const
{
  juce::XmlElement* child = parent.getChildByName(id);
  if (!child) { return; }

  juce::String value = child->getAllSubText();
  if (value.isEmpty()) { return; }

  set<bool>(id, value == "true");
}

template<>
inline void XmlParameters::read<int>(const std::string& id, juce::XmlElement& parent) const
{
  juce::XmlElement* child = parent.getChildByName(id);
  if (!child) { return; }

  juce::String value = child->getAllSubText();
  if (value.isEmpty()) { return; }

  set<int>(id, value.getIntValue());
}

template<>
inline void XmlParameters::read<float>(const std::string& id, juce::XmlElement& parent) const
{
  juce::XmlElement* child = parent.getChildByName(id);
  if (!child) { return; }

  juce::String value = child->getAllSubText();
  if (value.isEmpty()) { return; }

  set<float>(id, value.getFloatValue());
}

template<>
inline void XmlParameters::read<std::string>(const std::string& id, juce::XmlElement& parent) const
{
  juce::XmlElement* child = parent.getChildByName(id);
  if (!child) { return; }

  juce::String value = child->getAllSubText();
  if (value.isEmpty()) { return; }

  set<std::string>(id, value.toStdString());
}

template<>
inline void XmlParameters::write<bool>(const std::string& id, juce::XmlElement& parent) const
{
  juce::XmlElement* child = parent.createNewChildElement(id);
  child->addTextElement(get<bool>(id) ? "true" : "false");
}

template<>
inline void XmlParameters::write<int>(const std::string& id, juce::XmlElement& parent) const
{
  juce::XmlElement* child = parent.createNewChildElement(id);
  child->addTextElement(juce::String(get<int>(id)));
}

template<>
inline void XmlParameters::write<float>(const std::string& id, juce::XmlElement& parent) const
{
  juce::XmlElement* child = parent.createNewChildElement(id);
  child->addTextElement(juce::String(get<float>(id)));
}

template<>
inline void XmlParameters::write<std::string>(const std::string& id, juce::XmlElement& parent) const
{
  juce::XmlElement* child = parent.createNewChildElement(id);
  child->addTextElement(juce::String(get<std::string>(id)));
}

inline void XmlParameters::load(const void* data, const int size,
                                std::optional<std::function<void(const std::string& xml)>> callback)
{
  auto xml = std::unique_ptr<juce::XmlElement>(
    juce::AudioProcessor::getXmlFromBinary(data, size));

  if (!xml)
  {
    return;
  }

  if (callback)
  {
    const auto format = juce::XmlElement::TextFormat().withoutHeader();
    const auto text = xml->toString(format).toStdString();

    (*callback)(text);
  }

  if (xml->hasTagName(tag) != true) { return; }
  if (xml->getIntAttribute("schema") != schema) { return; }

  visit([&](const std::string& id, const AnyRangedAudioParameter& parameter)
  {
    std::visit(visitor{
      [&](juce::AudioParameterBool*)   { read<bool>(id, *xml);        },
      [&](juce::AudioParameterInt*)    { read<int>(id, *xml);         },
      [&](juce::AudioParameterFloat*)  { read<float>(id, *xml);       },
      [&](juce::AudioParameterChoice*) { read<std::string>(id, *xml); }
    }, parameter);
  });
}

inline void XmlParameters::save(juce::MemoryBlock& data,
                                std::optional<std::function<void(const std::string& xml)>> callback)
{
  auto xml = std::make_unique<juce::XmlElement>(juce::String(tag));

  xml->setAttribute("schema", schema);

  visit([&](const std::string& id, const AnyRangedAudioParameter& parameter)
  {
    std::visit(visitor{
      [&](juce::AudioParameterBool*)   { write<bool>(id, *xml);        },
      [&](juce::AudioParameterInt*)    { write<int>(id, *xml);         },
      [&](juce::AudioParameterFloat*)  { write<float>(id, *xml);       },
      [&](juce::AudioParameterChoice*) { write<std::string>(id, *xml); }
    }, parameter);
  });

  if (callback)
  {
    const auto format = juce::XmlElement::TextFormat().withoutHeader();
    const auto text = xml->toString(format).toStdString();

    (*callback)(text);
  }

  juce::AudioProcessor::copyXmlToBinary(*xml, data);
}

END_JUCE_JINGLES_NAMESPACE
