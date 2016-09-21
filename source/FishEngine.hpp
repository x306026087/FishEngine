#ifndef FishEngine_h
#define FishEngine_h

#if defined(_WIN32) || defined(_WIN64)
#define FISHENGINE_PLATFORM_WINDOWS 1
#elif defined(__APPLE__)
#define FISHENGINE_PLATFORM_APPLE 1
#else //defined(__linux__)
#define FISHENGINE_PLATFORM_LINUX 1
#endif

#define NAMESPACE_FISHENGINE_BEGIN namespace FishEngine {
#define NAMESPACE_FISHENGINE_END }

#include <string>
#include <vector>
#include <map>
#include <list>
#include <memory>

NAMESPACE_FISHENGINE_BEGIN

class Behaviour;
class Camera;
class Component;
class MeshFilter;
class MeshRenderer;
class Script;
class Renderer;
class Transform;
class Debug;
class GameObject;
class GameLoop;
class Input;
class Material;
class Mesh;
class Object;
class Shader;
class Texture;
class Time;
class App;

NAMESPACE_FISHENGINE_END

// hack: inject FishEditor namespace
namespace FishEditor {
    class EditorGUI;
    class EditorRenderSystem;
}

#endif /* FishEngine_h */