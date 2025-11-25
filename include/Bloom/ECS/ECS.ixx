module;

#include <atomic>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <iostream>
#include <cassert>
#include <shared_mutex>
#include <mutex>
#include <queue>
#include <functional>

export module Bloom.ECS;

export namespace ECS {
    using EntityID = std::size_t;

    struct Component {
        virtual ~Component() = default;
    };

    template<typename T>
    class ReadOnlyComponentArray {
    public:
        struct Snapshot {
            std::vector<T> components;
            std::vector<ECS::EntityID> indexToEntity;
            std::unordered_map<ECS::EntityID, std::size_t> entityToIndex; // optional, keeps API parity
        };

        ReadOnlyComponentArray() = default;
        explicit ReadOnlyComponentArray(std::shared_ptr<const Snapshot> s) : snap(std::move(s)) {}

        std::size_t getSize() const { return snap ? snap->components.size() : 0; }
        ECS::EntityID getEntity(std::size_t index) const {
            assert(snap && index < snap->indexToEntity.size());
            return snap->indexToEntity[index];
        }
        const T& getByIndex(std::size_t index) const {
            assert(snap && index < snap->components.size());
            return snap->components[index];
        }
        const T& get(ECS::EntityID entity) const {
            assert(snap);
            auto it = snap->entityToIndex.find(entity);
            assert(it != snap->entityToIndex.end());
            return snap->components[it->second];
        }
        bool has(ECS::EntityID entity) const {
            return snap && snap->entityToIndex.find(entity) != snap->entityToIndex.end();
        }
    private:
        std::shared_ptr<const Snapshot> snap;
    };

    template<typename T>
    class ComponentArray {
    private:
        std::vector<T> components;
        std::vector<EntityID> indexToEntity;               // dense, size == size
        std::unordered_map<EntityID, std::size_t> entityToIndex;
        std::size_t size = 0;

        mutable std::shared_mutex mutex;

        using Snapshot = typename ReadOnlyComponentArray<T>::Snapshot;
        std::atomic<std::shared_ptr<const Snapshot>> publishedSnapshot;

    public:
        ComponentArray() {
            // publish an empty snapshot initially
            auto empty = std::make_shared<Snapshot>();
            publishedSnapshot.store(empty, std::memory_order_release);
        }

        void insert(EntityID entity, const T& component) {
            std::unique_lock lock(mutex);
            assert(entityToIndex.find(entity) == entityToIndex.end() && "Component added twice to same entity.");

            std::size_t newIndex = size;
            entityToIndex[entity] = newIndex;

            if (newIndex == components.size()) {
                components.push_back(component);
                indexToEntity.push_back(entity);
            }
            else {
                components[newIndex] = component;
                indexToEntity[newIndex] = entity;
            }
            ++size;
        }

        void remove(EntityID entity) {
            std::unique_lock lock(mutex);
            auto it = entityToIndex.find(entity);
            assert(it != entityToIndex.end() && "Removing non-existent component.");
            if (it == entityToIndex.end()) return;

            std::size_t indexOfRemoved = it->second;
            std::size_t indexOfLast = size - 1;

            if (indexOfRemoved != indexOfLast) {
                components[indexOfRemoved] = std::move(components[indexOfLast]);
                EntityID lastEntity = indexToEntity[indexOfLast];
                indexToEntity[indexOfRemoved] = lastEntity;
                entityToIndex[lastEntity] = indexOfRemoved;
            }

            entityToIndex.erase(it);
            --size;
            // no need to shrink vectors each time; size denotes valid range
        }

        T& get(EntityID entity) {
            std::shared_lock lock(mutex);
            auto it = entityToIndex.find(entity);
            assert(it != entityToIndex.end() && "Retrieving non-existent component.");
            return components[it->second];
        }

        bool has(EntityID entity) const {
            std::shared_lock lock(mutex);
            return entityToIndex.find(entity) != entityToIndex.end();
        }

        std::size_t getSize() const {
            std::shared_lock lock(mutex);
            return size;
        }

        EntityID getEntity(std::size_t index) const {
            std::shared_lock lock(mutex);
            assert(index < size && "Index out of bounds");
            return indexToEntity[index];
        }

        T& getByIndex(std::size_t index) {
            std::shared_lock lock(mutex);
            assert(index < size && "Index out of bounds");
            return components[index];
        }

        // Build and publish a read-only snapshot.
        void publishSnapshot() {
            // No updates should be happening while publish is called in your pipeline,
            // but take a shared_lock to read a consistent view cheaply.
            std::shared_lock lock(mutex);

            auto snap = std::make_shared<Snapshot>();
            snap->components.assign(components.begin(), components.begin() + size);
            snap->indexToEntity.assign(indexToEntity.begin(), indexToEntity.begin() + size);

            // Rebuild entityToIndex cheaply without copying full unordered_map
            snap->entityToIndex.reserve(size * 2);
            for (std::size_t i = 0; i < size; ++i) {
                snap->entityToIndex.emplace(snap->indexToEntity[i], i);
            }

            publishedSnapshot.store(std::move(snap), std::memory_order_release);
        }

        ReadOnlyComponentArray<T> getReadOnly() const {
            return ReadOnlyComponentArray<T>(publishedSnapshot.load(std::memory_order_acquire));
        }

        void forEach(std::function<void(EntityID, T&)> func) {
            std::shared_lock lock(mutex);
            for (std::size_t i = 0; i < size; ++i) {
                func(indexToEntity[i], components[i]);
            }
        }
    };

    class IComponentArray {
    public:
        virtual ~IComponentArray() = default;
        virtual void entityDestroyed(EntityID entity) = 0;
        virtual void publishSnapshot() = 0; // new
    };

    template<typename T>
    class ComponentArrayWrapper : public IComponentArray {
    public:
        ComponentArray<T> array;

        void entityDestroyed(EntityID entity) override {
            if (array.has(entity)) {
                array.remove(entity);
            }
        }

        void publishSnapshot() override {
            array.publishSnapshot();
        }
    };

    class ComponentManager {
    private:
        std::unordered_map<std::type_index, std::unique_ptr<IComponentArray>> componentArrays;
        mutable std::shared_mutex mutex;

        template<typename T>
        ComponentArray<T>& getComponentArray() {
            std::type_index typeName = std::type_index(typeid(T));

            {
                std::shared_lock lock(mutex);
                auto it = componentArrays.find(typeName);
                if (it != componentArrays.end()) {
                    return static_cast<ComponentArrayWrapper<T>*>(it->second.get())->array;
                }
            }
            std::unique_lock lock(mutex);
            auto it = componentArrays.find(typeName);
            if (it == componentArrays.end()) {
                componentArrays[typeName] = std::make_unique<ComponentArrayWrapper<T>>();
            }
            return static_cast<ComponentArrayWrapper<T>*>(componentArrays[typeName].get())->array;
        }

    public:
        template<typename T>
        void addComponent(EntityID entity, const T& component) { getComponentArray<T>().insert(entity, component); }

        template<typename T>
        void removeComponent(EntityID entity) { getComponentArray<T>().remove(entity); }

        template<typename T>
        T& getComponent(EntityID entity) { return getComponentArray<T>().get(entity); }

        template<typename T>
        bool hasComponent(EntityID entity) { return getComponentArray<T>().has(entity); }

        template<typename T>
        ComponentArray<T>& getArray() { return getComponentArray<T>(); }

        template<typename T>
        ReadOnlyComponentArray<T> getReadOnlyArray() const {
            // const-cast is safe here; we're just reading its snapshot
            auto& arr = const_cast<ComponentManager*>(this)->getComponentArray<T>();
            return arr.getReadOnly();
        }

        void publishAllSnapshots() {
            std::shared_lock lock(mutex);
            for (auto& kv : componentArrays) {
                kv.second->publishSnapshot();
            }
        }

        void entityDestroyed(EntityID entity) {
            std::shared_lock lock(mutex);
            for (auto& pair : componentArrays) {
                pair.second->entityDestroyed(entity);
            }
        }
    };

    // Entity Manager
    class EntityManager {
    private:
        std::vector<EntityID> availableEntities;
        EntityID nextEntityID = 0;
        mutable std::mutex mutex;

    public:
        EntityID createEntity() {
            std::lock_guard lock(mutex);
            EntityID id;

            if (availableEntities.empty()) {
                id = nextEntityID++;
            }
            else {
                id = availableEntities.back();
                availableEntities.pop_back();
            }

            return id;
        }

        void destroyEntity(EntityID entity) {
            std::lock_guard lock(mutex);
            availableEntities.push_back(entity);
        }
    };

    // System base class
    class System {
    public:
        virtual ~System() = default;
        virtual void update(double deltaTime) = 0;
    };

    // Deferred operation types
    enum class OperationType {
        CREATE_ENTITY,
        DESTROY_ENTITY,
        ADD_COMPONENT,
        REMOVE_COMPONENT
    };

    struct DeferredOperation {
        OperationType type;
        EntityID entity;
        std::type_index componentType;
        std::function<void()> operation;

        DeferredOperation(OperationType t, EntityID e, std::type_index ct, std::function<void()> op)
            : type(t), entity(e), componentType(ct), operation(std::move(op)) {
        }
    };

    // Main ECS World
    class World {
    private:
        std::unique_ptr<EntityManager> entityManager;
        std::unique_ptr<ComponentManager> componentManager;
        std::vector<std::unique_ptr<System>> systems;
        float time = 0;

        // Deferred operations queue
        std::queue<DeferredOperation> pendingOperations;
        mutable std::mutex operationsMutex;
        std::atomic<bool> isUpdating{ false };

        void processPendingOperations() {
            std::lock_guard lock(operationsMutex);
            while (!pendingOperations.empty()) {
                auto& op = pendingOperations.front();
                op.operation();
                pendingOperations.pop();
            }
        }

    public:
        World() {
            entityManager = std::make_unique<EntityManager>();
            componentManager = std::make_unique<ComponentManager>();
        }

        EntityID createEntity() {
            if (isUpdating.load()) {
                // Defer operation if update is running
                EntityID futureEntity = entityManager->createEntity();
                return futureEntity;
            }
            return entityManager->createEntity();
        }

        void destroyEntity(EntityID entity) {
            if (isUpdating.load()) {
                std::lock_guard lock(operationsMutex);
                pendingOperations.emplace(
                    OperationType::DESTROY_ENTITY,
                    entity,
                    std::type_index(typeid(void)),
                    [this, entity]() {
                        componentManager->entityDestroyed(entity);
                        entityManager->destroyEntity(entity);
                    }
                );
                return;
            }

            componentManager->entityDestroyed(entity);
            entityManager->destroyEntity(entity);
        }

        template<typename T>
        void addComponent(EntityID entity, T component) {
            if (isUpdating.load()) {
                // Defer operation if update is running
                std::lock_guard lock(operationsMutex);
                pendingOperations.emplace(
                    OperationType::ADD_COMPONENT,
                    entity,
                    std::type_index(typeid(T)),
                    [this, entity, component]() {
                        componentManager->addComponent<T>(entity, component);
                    }
                );
                return;
            }

            componentManager->addComponent<T>(entity, component);
        }

        template<typename T>
        void removeComponent(EntityID entity) {
            if (isUpdating.load()) {
                // Defer operation if update is running
                std::lock_guard lock(operationsMutex);
                pendingOperations.emplace(
                    OperationType::REMOVE_COMPONENT,
                    entity,
                    std::type_index(typeid(T)),
                    [this, entity]() {
                        componentManager->removeComponent<T>(entity);
                    }
                );
                return;
            }

            componentManager->removeComponent<T>(entity);
        }

        template<typename T>
        T& getComponent(EntityID entity) {
            return componentManager->getComponent<T>(entity);
        }

        template<typename T>
        bool hasComponent(EntityID entity) {
            return componentManager->hasComponent<T>(entity);
        }

        template<typename T>
        ComponentArray<T>& getComponentArray() {
            return componentManager->getArray<T>();
        }

        template<typename T>
        void registerSystem(std::unique_ptr<T> system) {
            systems.push_back(std::move(system));
        }

        void update(float deltaTime) {
            if (deltaTime <= 0) return;

            // Process any pending ops from earlier frames (e.g., created from render thread)
            processPendingOperations();

            isUpdating.store(true);
            time += deltaTime;

            for (auto& system : systems) {
                system->update(deltaTime); // systems mutate writable arrays
            }

            isUpdating.store(false);

            // Publish a snapshot of the current world for the render thread
            componentManager->publishAllSnapshots();

            // Now apply any operations that were deferred during this update.
            // Those will appear in the NEXT frame's snapshot.
            processPendingOperations();
        }

        float getTime() {
            return time;
        }
    
        template<typename T>
        ReadOnlyComponentArray<T> getReadOnlyComponentArray() const {
            return componentManager->getReadOnlyArray<T>();
        }
    };
}