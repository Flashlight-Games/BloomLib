#include <atomic>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <cassert>
#include <queue>
#include <functional>
#include <mutex>

export module Bloom.ECS;

import std;

export namespace ECS {
    using EntityID = std::size_t;

    struct Component {
        virtual ~Component() = default;
    };

    // ============================================
    // READ-ONLY SNAPSHOT (for main/render thread)
    // ============================================
    template<typename T>
    class ReadOnlyComponentArray {
    public:
        struct Snapshot {
            std::vector<T> components;
            std::vector<EntityID> entities;
            std::unordered_map<EntityID, std::size_t> entityToIndex;
        };

        ReadOnlyComponentArray() = default;
        explicit ReadOnlyComponentArray(std::shared_ptr<const Snapshot> s) : snap(std::move(s)) {}

        std::size_t size() const {
            return snap ? snap->components.size() : 0;
        }

        const T& operator[](std::size_t index) const {
            assert(snap && index < snap->components.size());
            return snap->components[index];
        }

        EntityID getEntity(std::size_t index) const {
            assert(snap && index < snap->entities.size());
            return snap->entities[index];
        }

        bool has(EntityID entity) const {
            return snap && snap->entityToIndex.find(entity) != snap->entityToIndex.end();
        }

        const T* tryGet(EntityID entity) const {
            if (!snap) return nullptr;
            auto it = snap->entityToIndex.find(entity);
            return it != snap->entityToIndex.end() ? &snap->components[it->second] : nullptr;
        }

        template <typename Func>
        const T* tryGet(EntityID entity, Func&& func) const {
            const T* component = tryGet(entity);
            if (component)
                func(component);
            return component;
        }

        // Iterate over snapshot
        template<typename Func>
        void forEach(Func&& func) const {
            if (!snap) return;
            for (std::size_t i = 0; i < snap->components.size(); ++i) {
                if constexpr (std::is_invocable_r_v<bool, Func, EntityID, const T&>) {
                    // Func returns bool - early termination support
                    if (!func(snap->entities[i], snap->components[i])) {
                        break;
                    }
                }
                else {
                    // Func returns void - regular iteration
                    func(snap->entities[i], snap->components[i]);
                }
            }
        }

    private:
        std::shared_ptr<const Snapshot> snap;
    };

    // ============================================
    // WRITABLE COMPONENT ARRAY (system thread only)
    // ============================================
    template<typename T>
    class ComponentArray {
    private:
        std::vector<T> components;
        std::vector<EntityID> entities;
        std::unordered_map<EntityID, std::size_t> entityToIndex;

        using Snapshot = typename ReadOnlyComponentArray<T>::Snapshot;
        std::atomic<std::shared_ptr<const Snapshot>> publishedSnapshot;

    public:
        ComponentArray() {
            auto empty = std::make_shared<Snapshot>();
            publishedSnapshot.store(empty, std::memory_order_release);
        }

        void add(EntityID entity, const T& component) {
            assert(entityToIndex.find(entity) == entityToIndex.end() && "Component already exists");

            std::size_t index = components.size();
            entityToIndex[entity] = index;
            components.push_back(component);
            entities.push_back(entity);
        }

        void remove(EntityID entity) {
            auto it = entityToIndex.find(entity);
            if (it == entityToIndex.end()) return;

            std::size_t removedIndex = it->second;
            std::size_t lastIndex = components.size() - 1;

            // Swap with last element
            if (removedIndex != lastIndex) {
                components[removedIndex] = std::move(components[lastIndex]);
                entities[removedIndex] = entities[lastIndex];
                entityToIndex[entities[removedIndex]] = removedIndex;
            }

            components.pop_back();
            entities.pop_back();
            entityToIndex.erase(it);
        }

        T& get(EntityID entity) {
            auto it = entityToIndex.find(entity);
            assert(it != entityToIndex.end() && "Component does not exist");
            return components[it->second];
        }

        bool has(EntityID entity) const {
            return entityToIndex.find(entity) != entityToIndex.end();
        }

        T* tryGet(EntityID entity) {
            auto it = entityToIndex.find(entity);
            return it != entityToIndex.end() ? &components[it->second] : nullptr;
        }

        std::size_t size() const {
            return components.size();
        }

        T& operator[](std::size_t index) {
            assert(index < components.size());
            return components[index];
        }

        EntityID getEntity(std::size_t index) const {
            assert(index < entities.size());
            return entities[index];
        }

        // Publish snapshot for main thread
        void publishSnapshot() {
            auto snap = std::make_shared<Snapshot>();
            snap->components = components;
            snap->entities = entities;
            snap->entityToIndex = entityToIndex;
            publishedSnapshot.store(std::move(snap), std::memory_order_release);
        }

        ReadOnlyComponentArray<T> getReadOnly() const {
            return ReadOnlyComponentArray<T>(publishedSnapshot.load(std::memory_order_acquire));
        }
    };

    // ============================================
    // COMPONENT MANAGER (internal)
    // ============================================
    class IComponentArray {
    public:
        virtual ~IComponentArray() = default;
        virtual void removeEntity(EntityID entity) = 0;
        virtual void publishSnapshot() = 0;
    };

    template<typename T>
    class ComponentArrayWrapper : public IComponentArray {
    public:
        ComponentArray<T> array;

        void removeEntity(EntityID entity) override {
            array.remove(entity);
        }

        void publishSnapshot() override {
            array.publishSnapshot();
        }
    };

    class ComponentManager {
    private:
        std::unordered_map<std::type_index, std::unique_ptr<IComponentArray>> arrays;

        template<typename T>
        ComponentArray<T>& getOrCreateArray() {
            std::type_index typeIndex(typeid(T));
            auto it = arrays.find(typeIndex);

            if (it == arrays.end()) {
                arrays[typeIndex] = std::make_unique<ComponentArrayWrapper<T>>();
                it = arrays.find(typeIndex);
            }

            return static_cast<ComponentArrayWrapper<T>*>(it->second.get())->array;
        }

    public:
        template<typename T>
        void add(EntityID entity, const T& component) {
            getOrCreateArray<T>().add(entity, component);
        }

        template<typename T>
        void remove(EntityID entity) {
            getOrCreateArray<T>().remove(entity);
        }

        template<typename T>
        T& get(EntityID entity) {
            return getOrCreateArray<T>().get(entity);
        }

        template<typename T>
        bool has(EntityID entity) {
            return getOrCreateArray<T>().has(entity);
        }

        template<typename T>
        T* tryGet(EntityID entity) {
            return getOrCreateArray<T>().tryGet(entity);
        }

        template<typename T>
        ComponentArray<T>& getArray() {
            return getOrCreateArray<T>();
        }

        template<typename T>
        ReadOnlyComponentArray<T> getReadOnlyArray() {
            return getOrCreateArray<T>().getReadOnly();
        }

        void removeEntity(EntityID entity) {
            for (auto& [type, array] : arrays) {
                array->removeEntity(entity);
            }
        }

        void publishAllSnapshots() {
            for (auto& [type, array] : arrays) {
                array->publishSnapshot();
            }
        }
    };

    // ============================================
    // ENTITY MANAGER (internal)
    // ============================================
    class EntityManager {
    private:
        std::vector<EntityID> recycled;
        EntityID nextID = 0;

    public:
        EntityID create() {
            if (recycled.empty()) {
                return nextID++;
            }
            EntityID id = recycled.back();
            recycled.pop_back();
            return id;
        }

        void destroy(EntityID entity) {
            recycled.push_back(entity);
        }

        void createWithID(EntityID id) {
            auto it = std::find(recycled.begin(), recycled.end(), id);
            if (it != recycled.end()) {
                recycled.erase(it);
            }
            if (id >= nextID) {
                nextID = id + 1;
            }
        }
    };

    // ============================================
    // SYSTEM BASE CLASS
    // ============================================
    class SystemContext {
    private:
        SystemContext() = default;
        friend class World;
    };

    class System {
    public:
        virtual ~System() = default;
        virtual void update(double deltaTime, SystemContext& ctx) = 0;
    };

    // ============================================
    // WORLD (main ECS interface)
    // ============================================
    class World {
    private:
        EntityManager entityManager;
        ComponentManager componentManager;
        std::vector<std::unique_ptr<System>> systems;
        float time = 0;

        // Command queue for main thread -> system thread
        struct Command {
            std::function<void()> execute;
        };
        std::queue<Command> commandQueue;
        std::mutex commandMutex;

        // Separate ID space for queued entities
        std::atomic<EntityID> queuedEntityCounter{ 1000000 };

        void processCommands() {
            std::lock_guard lock(commandMutex);
            while (!commandQueue.empty()) {
                commandQueue.front().execute();
                commandQueue.pop();
            }
        }

        void queueCommand(std::function<void()> fn) {
            std::lock_guard lock(commandMutex);
            commandQueue.push(Command{ std::move(fn) });
        }

    public:
        // ========================================
        // MAIN THREAD API (queue operations, read snapshots)
        // ========================================

        EntityID queueCreateEntity() {
            EntityID id = queuedEntityCounter.fetch_add(1, std::memory_order_relaxed);
            queueCommand([this, id]() {
                entityManager.createWithID(id);
                });
            return id;
        }

        template<typename T>
        void queueAddComponent(EntityID entity, T component) {
            queueCommand([this, entity, comp = std::move(component)]() mutable {
                componentManager.add<T>(entity, std::move(comp));
                });
        }

        template<typename T>
        void queueRemoveComponent(EntityID entity) {
            queueCommand([this, entity]() {
                componentManager.remove<T>(entity);
                });
        }

        template<typename T>
        void queueUpdateComponent(EntityID entity, std::function<void(T&)> updateFn) {
            queueCommand([this, entity, updateFn = std::move(updateFn)]() {
                if (auto* comp = componentManager.tryGet<T>(entity)) {
                    updateFn(*comp);
                }
                });
        }

        void queueDestroyEntity(EntityID entity) {
            queueCommand([this, entity]() {
                componentManager.removeEntity(entity);
                entityManager.destroy(entity);
                });
        }

        template<typename T>
        ReadOnlyComponentArray<T> getSnapshot() {
            return componentManager.getReadOnlyArray<T>();
        }

        // ========================================
        // SYSTEM THREAD API (direct access, requires SystemContext)
        // ========================================

        EntityID createEntity(SystemContext&) {
            return entityManager.create();
        }

        void destroyEntity(EntityID entity, SystemContext&) {
            componentManager.removeEntity(entity);
            entityManager.destroy(entity);
        }

        template<typename T>
        void addComponent(EntityID entity, const T& component, SystemContext&) {
            componentManager.add<T>(entity, component);
        }

        template<typename T>
        void removeComponent(EntityID entity, SystemContext&) {
            componentManager.remove<T>(entity);
        }

        template<typename T>
        T& getComponent(EntityID entity, SystemContext&) {
            return componentManager.get<T>(entity);
        }

        template<typename T>
        bool hasComponent(EntityID entity, SystemContext&) {
            return componentManager.has<T>(entity);
        }

        template<typename T>
        T* tryGetComponent(EntityID entity, SystemContext&) {
            return componentManager.tryGet<T>(entity);
        }

        template<typename T>
        ComponentArray<T>& getArray(SystemContext&) {
            return componentManager.getArray<T>();
        }

        // Clean iteration: loop over entities with required component(s)
        // Supports both void and bool-returning lambdas (bool for early termination)
        template<typename T, typename Func>
        void forEach(SystemContext&, Func&& func) {
            auto& array = componentManager.getArray<T>();
            for (std::size_t i = 0; i < array.size(); ++i) {
                if constexpr (std::is_invocable_r_v<bool, Func, EntityID, T&>) {
                    // Func returns bool - early termination support
                    if (!func(array.getEntity(i), array[i])) {
                        break;
                    }
                }
                else {
                    // Func returns void - regular iteration
                    func(array.getEntity(i), array[i]);
                }
            }
        }

        // Iteration with multiple required components
        // Supports both void and bool-returning lambdas
        template<typename T1, typename T2, typename... Rest, typename Func>
        void forEach(SystemContext&, Func&& func) {
            auto& array1 = componentManager.getArray<T1>();

            for (std::size_t i = 0; i < array1.size(); ++i) {
                EntityID entity = array1.getEntity(i);

                // Check if entity has all required components
                if (componentManager.has<T2>(entity) && (componentManager.has<Rest>(entity) && ...)) {
                    if constexpr (std::is_invocable_r_v<bool, Func, EntityID, T1&, T2&, Rest&...>) {
                        // Early termination support
                        if (!func(entity, array1[i], componentManager.get<T2>(entity), componentManager.get<Rest>(entity)...)) {
                            break;
                        }
                    }
                    else {
                        // Regular iteration
                        func(entity, array1[i], componentManager.get<T2>(entity), componentManager.get<Rest>(entity)...);
                    }
                }
            }
        }

        // ========================================
        // SYSTEM MANAGEMENT
        // ========================================

        template<typename T, typename... Args>
        T* addSystem(Args&&... args) {
            auto system = std::make_unique<T>(this, std::forward<Args>(args)...);
            T* ptr = system.get();
            systems.push_back(std::move(system));
            return ptr;
        }

        void update(float deltaTime) {
            if (deltaTime <= 0) return;

            // Process commands from main thread
            processCommands();

            time += deltaTime;

            // Run systems
            SystemContext ctx;
            for (auto& system : systems) {
                system->update(deltaTime, ctx);
            }

            // Publish snapshots for main thread
            componentManager.publishAllSnapshots();
        }

        float getTime() const {
            return time;
        }
    };
}