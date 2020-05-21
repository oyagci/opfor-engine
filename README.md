# 3D Engine
Game Engine in C++ and OpenGL

## Examples
### Creating a Component
```cpp
#include "ecs/Component.h"

struct MyComponent : ecs::ComponentBase
{
  // Data fields ...
  int Health;
  float Speed;
  // ...
};
```
### Creating an Entity
```cpp
auto &engine = Engine::Instance();

/// Create an IEntity with components MeshComponent and TransformComponent and returns a non-owning pointer
auto object = engine.CreateEntity<MeshComponent, TransformComponent>();
```
### Modifying an Entity
```cpp
/// IEntity::GetAll returns a tuple with the components registered in the IEntity
auto [ mesh, transform ] = object->GetAll();

transform.position = glm::vec3(42.0f, 0.0f, 0.0f);
mesh->Set(transform);
// or (without type deduction)
mesh->Set<TransformComponent>(transform);
```
### Creating a System
```cpp
#include "ecs/System.hpp"

class MySystem : ecs::ComponentSystem
{
public:
  void OnUpdate(float) override
  {
    auto entities = GetEntities<MeshComponent, TransformComponent>();
    // Do your computations here...
  }
};
```
### Instanciating a System
```cpp
engine.CreateSystem<MySystem>();
```
## Build

### Linux, macOS
```cpp
meson build && ninja -C build
```

## Dependencies

### Common
```
glew
glfw3
FreeType
OpenGL
OpenAL
```

### Linux
```
x11
xi
xrandr
```

(Light Bulb icon)[https://icons8.com/icons/set/blunt-bulb] icon by (Icons8)[https://icons8.com]
