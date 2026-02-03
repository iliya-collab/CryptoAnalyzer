#include "Engine/Registry.hpp"

void Engine::Registry::Register(const QString& name, const QString& coin) {
    m_registry[name].insert(coin);
}

void Engine::Registry::Register(const QString& coin) {
    m_registry[coin];
}

bool Engine::Registry::hasRegistered(const QString& name, const QString& coin) {
    auto it = m_registry.find(name);
    if (it != m_registry.end())
        return it.value().contains(coin);
    return false;
}

bool Engine::Registry::hasRegistered(const QString& name) {
    return m_registry.contains(name);
}

bool Engine::Registry::hasCoins(const QString& name) {
    auto it = m_registry.find(name);
    return it != m_registry.end() && !it.value().isEmpty();
}

void Engine::Registry::Delete(const QString& name, const QString& coin) {
    auto it = m_registry.find(name);
    if (it != m_registry.end())
        it.value().remove(coin);
}

void Engine::Registry::Delete(const QString& name) {
    m_registry.remove(name);
}

QSet<QString> Engine::Registry::getRegisteredCoins(const QString& name) {
    return m_registry.value(name);
}