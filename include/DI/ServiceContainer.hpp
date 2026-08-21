#pragma once

#include <memory>
#include <unordered_map>
#include <typeindex>
#include <functional>
#include "Error/Result.hpp"
#include "Error/ErrorHandler.hpp"

/**
 * @brief Simple dependency injection container for embedded systems
 * 
 * Provides lightweight dependency injection without excessive memory overhead.
 * Supports singleton, transient, and scoped lifetimes.
 */
enum class ServiceLifetime {
    Singleton,    // One instance for the lifetime of the container
    Transient,    // New instance every time
    Scoped        // One instance per scope (not implemented in this minimal version)
};

/**
 * @brief Service descriptor for registration
 */
struct ServiceDescriptor {
    std::function<std::shared_ptr<void>()> factory;
    ServiceLifetime lifetime;
    std::shared_ptr<void> instance;  // For singleton instances
    
    ServiceDescriptor(std::function<std::shared_ptr<void>()> f, ServiceLifetime l)
        : factory(f), lifetime(l) {}
};

/**
 * @brief Lightweight dependency injection container
 */
class ServiceContainer {
private:
    std::unordered_map<std::type_index, ServiceDescriptor> _services;
    bool _sealed;  // Prevent registration after first resolve

public:
    ServiceContainer() : _sealed(false) {}
    
    /**
     * @brief Register a service with a factory function
     * @tparam Interface Service interface type
     * @tparam Implementation Service implementation type
     * @param lifetime Service lifetime
     */
    template<typename Interface, typename Implementation>
    void registerService(ServiceLifetime lifetime = ServiceLifetime::Singleton) {
        if (_sealed) {
            ErrorHandler::handleError(ErrorCode::InvalidState, ErrorSeverity::Error, 
                                     "Cannot register services after container is sealed", 
                                     "ServiceContainer");
            return;
        }
        
        std::type_index key = std::type_index(typeid(Interface));
        
        auto factory = [this]() -> std::shared_ptr<void> {
            return std::static_pointer_cast<void>(std::make_shared<Implementation>());
        };
        
        _services[key] = ServiceDescriptor(factory, lifetime);
        
        LOG_DEBUG_CTX("Registered service: " + String(typeid(Interface).name()), "ServiceContainer");
    }
    
    /**
     * @brief Register a service with a custom factory
     * @tparam Interface Service interface type
     * @param factory Factory function
     * @param lifetime Service lifetime
     */
    template<typename Interface>
    void registerService(std::function<std::shared_ptr<Interface>()> factory, 
                        ServiceLifetime lifetime = ServiceLifetime::Singleton) {
        if (_sealed) {
            ErrorHandler::handleError(ErrorCode::InvalidState, ErrorSeverity::Error, 
                                     "Cannot register services after container is sealed", 
                                     "ServiceContainer");
            return;
        }
        
        std::type_index key = std::type_index(typeid(Interface));
        
        auto voidFactory = [factory]() -> std::shared_ptr<void> {
            return std::static_pointer_cast<void>(factory());
        };
        
        _services[key] = ServiceDescriptor(voidFactory, lifetime);
        
        LOG_DEBUG_CTX("Registered service with factory: " + String(typeid(Interface).name()), "ServiceContainer");
    }
    
    /**
     * @brief Register a singleton instance
     * @tparam Interface Service interface type
     * @param instance Pre-created instance
     */
    template<typename Interface>
    void registerInstance(std::shared_ptr<Interface> instance) {
        if (_sealed) {
            ErrorHandler::handleError(ErrorCode::InvalidState, ErrorSeverity::Error, 
                                     "Cannot register services after container is sealed", 
                                     "ServiceContainer");
            return;
        }
        
        std::type_index key = std::type_index(typeid(Interface));
        
        auto factory = [instance]() -> std::shared_ptr<void> {
            return std::static_pointer_cast<void>(instance);
        };
        
        ServiceDescriptor descriptor(factory, ServiceLifetime::Singleton);
        descriptor.instance = std::static_pointer_cast<void>(instance);
        
        _services[key] = descriptor;
        
        LOG_DEBUG_CTX("Registered instance: " + String(typeid(Interface).name()), "ServiceContainer");
    }
    
    /**
     * @brief Resolve a service
     * @tparam Interface Service interface type
     * @return Shared pointer to the service, or nullptr if not found
     */
    template<typename Interface>
    std::shared_ptr<Interface> resolve() {
        _sealed = true;  // Seal after first resolve
        
        std::type_index key = std::type_index(typeid(Interface));
        auto it = _services.find(key);
        
        if (it == _services.end()) {
            ErrorHandler::handleError(ErrorCode::ServiceNotFound, ErrorSeverity::Error, 
                                     "Service not found: " + String(typeid(Interface).name()), 
                                     "ServiceContainer");
            return nullptr;
        }
        
        auto& descriptor = it->second;
        
        switch (descriptor.lifetime) {
            case ServiceLifetime::Singleton:
                if (!descriptor.instance) {
                    descriptor.instance = descriptor.factory();
                }
                return std::static_pointer_cast<Interface>(descriptor.instance);
                
            case ServiceLifetime::Transient:
                return std::static_pointer_cast<Interface>(descriptor.factory());
                
            case ServiceLifetime::Scoped:
                // Not implemented in this minimal version
                ErrorHandler::handleError(ErrorCode::InvalidState, ErrorSeverity::Warning, 
                                         "Scoped lifetime not implemented, using singleton", 
                                         "ServiceContainer");
                if (!descriptor.instance) {
                    descriptor.instance = descriptor.factory();
                }
                return std::static_pointer_cast<Interface>(descriptor.instance);
        }
        
        return nullptr;
    }
    
    /**
     * @brief Check if a service is registered
     * @tparam Interface Service interface type
     * @return true if service is registered
     */
    template<typename Interface>
    bool isRegistered() const {
        std::type_index key = std::type_index(typeid(Interface));
        return _services.find(key) != _services.end();
    }
    
    /**
     * @brief Clear all services (for testing)
     */
    void clear() {
        _services.clear();
        _sealed = false;
        LOG_DEBUG_CTX("Service container cleared", "ServiceContainer");
    }
    
    /**
     * @brief Get number of registered services
     * @return Number of services
     */
    size_t getServiceCount() const {
        return _services.size();
    }
    
    /**
     * @brief Check if container is sealed
     * @return true if sealed
     */
    bool isSealed() const {
        return _sealed;
    }
};

/**
 * @brief Global service locator (use sparingly)
 */
class ServiceLocator {
private:
    static std::unique_ptr<ServiceContainer> _container;

public:
    static void initialize(std::unique_ptr<ServiceContainer> container) {
        _container = std::move(container);
    }
    
    template<typename Interface>
    static std::shared_ptr<Interface> resolve() {
        if (!_container) {
            ErrorHandler::handleError(ErrorCode::InvalidState, ErrorSeverity::Critical, 
                                     "Service container not initialized", 
                                     "ServiceLocator");
            return nullptr;
        }
        return _container->resolve<Interface>();
    }
    
    static void shutdown() {
        _container.reset();
    }
    
    static bool isInitialized() {
        return _container != nullptr;
    }
};

// Static member definition
std::unique_ptr<ServiceContainer> ServiceLocator::_container = nullptr;
