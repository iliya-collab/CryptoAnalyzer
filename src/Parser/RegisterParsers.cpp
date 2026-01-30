#include "Parser/RegisterParsers.hpp"

void RegisterParsers::registerParser(const QString& name, const QString& channel) {
    m_registry[name].insert(channel);
}

void RegisterParsers::registerParser(const QString& name) {
    m_registry[name];
}

bool RegisterParsers::hasRegistered(const QString& name, const QString& channel) {
    auto it = m_registry.find(name);
    if (it != m_registry.end())
        return it.value().contains(channel);
    return false;
}

bool RegisterParsers::hasRegistered(const QString& name) {
    return m_registry.contains(name);
}

bool RegisterParsers::hasChannels(const QString& name) {
    auto it = m_registry.find(name);
    return it != m_registry.end() && !it.value().isEmpty();
}

void RegisterParsers::deleteParser(const QString& name, const QString& channel) {
    auto it = m_registry.find(name);
    if (it != m_registry.end())
        it.value().remove(channel);
}

void RegisterParsers::deleteParser(const QString& name) {
    m_registry.remove(name);
}