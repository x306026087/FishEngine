#include "EditorGUI.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>
#include <imgui/imgui_internal.h>

#include "GameObject.hpp"
#include "Scene.hpp"
#include "Material.hpp"
#include "MeshFilter.hpp"
#include "MeshRenderer.hpp"
#include "Mesh.hpp"
#include "Common.hpp"
#include "Debug.hpp"
//#include "RenderSettings.hpp"
#include "Selection.hpp"
#include "EditorRenderSystem.hpp"
#include <Time.hpp>
#include <sstream>

#include <imgui/imgui_dock.h>

using namespace FishEngine;

NAMESPACE_FISHEDITOR_BEGIN

int EditorGUI::m_idCount = 0;
bool EditorGUI::m_showAssectSelectionDialogBox = false;

Material::PMaterial sceneGizmoMaterial;
Mesh::PMesh cubeMesh;
Mesh::PMesh coneMesh;

void EditorGUI::Init()
{
#if FISHENGINE_PLATFORM_WINDOWS
    const std::string root_dir = "../../assets/";
#else
    const std::string root_dir = "/Users/yushroom/program/graphics/FishEngine/assets/";
#endif
    const std::string models_dir = root_dir + "models/";
 
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF((root_dir+"fonts/DroidSans.ttf").c_str(), 14.0f);
    
    ImGuiContext& g = *GImGui;
    ImGuiStyle& style = g.Style;
    style.FrameRounding = 4.f;
    style.WindowRounding = 0.f;
    //style.Colors[ImGuiCol_Text] = ImVec4(0, 0, 0, 1);
    //style.Colors[ImGuiCol_Button] = ImVec4(171/255.f, 204/255.f, 242/255.f, 1.f);
    //style.Colors[ImGuiCol_WindowBg] = ImVec4(0.8f, 0.8f, 0.8f, 1.f);
    //style.GrabRounding = 0.f;
    //style.WindowTitleAlign = ImGuiAlign_Left | ImGuiAlign_VCenter;
    style.WindowMinSize = ImVec2(256, 256);
    
    sceneGizmoMaterial = Material::builtinMaterial("VertexLit");
    cubeMesh = Mesh::CreateFromObjFile(models_dir+"cube.obj");
    coneMesh = Mesh::CreateFromObjFile(models_dir+"cone.obj");
}

void EditorGUI::Update()
{
    static double time_stamp = glfwGetTime();
    ImGuiContext& g = *GImGui;
    ImGuiStyle& style = g.Style;
    
    //ImGui::BeginDock("Dock");
    //ImGui::EndDock();
    
    // Inspector Editor
    //ImGui::BeginDock("Inspector", nullptr);
    ImGui::Begin("Inspector", nullptr);
    ImGui::PushItemWidth(ImGui::GetWindowWidth()*0.55f);
    auto go = Selection::activeGameObject();
    if (go != nullptr) {
        ImGui::PushID("Inspector.selected.active");
        ImGui::Checkbox("", &go->m_activeSelf);
        ImGui::PopID();
        char name[32] = {0};
        memcpy(name, go->name().c_str(), go->name().size());
        name[go->m_name.size()] = 0;
        ImGui::SameLine();
        ImGui::PushID("Inspector.selected.name");
        if (ImGui::InputText("", name, 31)) {
            go->m_name = name;
        }
        ImGui::PopID();

        ImGui::PushItemWidth(ImGui::GetWindowWidth()*0.3f);
        //ImGui::LabelText("", "Tag");
        ImGui::Text("Tag");
        ImGui::SameLine();
        ImGui::LabelText("##Tag", "%s", go->tag().c_str());
        ImGui::SameLine();
        ImGui::Text("Layer");
        ImGui::SameLine();
        ImGui::LabelText("##Layer", "Layer %d", go->m_layer);
        ImGui::PopItemWidth();
        
        if (ImGui::CollapsingHeader("Transform##header", ImGuiTreeNodeFlags_DefaultOpen)) {
            go->m_transform->OnInspectorGUI();
        }
        
        for (auto& c : go->m_components) {
            bool is_open = true;
            if (ImGui::CollapsingHeader((camelCaseToReadable(c->ClassName())+"##header").c_str(), &is_open, ImGuiTreeNodeFlags_DefaultOpen))
                c->OnInspectorGUI();
            if (!is_open) {
                Object::Destroy(c);
            }
        }
        for (auto& s : go->m_scripts) {
            bool is_open = true;
            if (ImGui::CollapsingHeader((camelCaseToReadable(s->ClassName())+"##header").c_str(), &is_open, ImGuiTreeNodeFlags_DefaultOpen))
                s->OnInspectorGUI();
            if (!is_open) {
                Object::Destroy(s);
            }
        }
    }
    
    EditorGUI::Button("Add Component");
    //ImGui::EndDock(); // Inspector Editor
    ImGui::End();
    
    glClear(GL_DEPTH_BUFFER_BIT);
    DrawSceneGizmo();
    
    // Main menu bar
//    if (ImGui::BeginMainMenuBar()) {
//        if (ImGui::BeginMenu("File")) {
//            ImGui::MenuItem("New Scene", "Ctrl+N");
//            ImGui::EndMenu();
//        }
//        if (ImGui::BeginMenu("Edit")) {
//            ImGui::EndMenu();
//        }
//        
//        double new_time = glfwGetTime();
//        int fps = (int)roundf(1.f / (new_time - time_stamp));
//        time_stamp = new_time;
//        std::ostringstream sout;
//        sout << "FPS: " << fps;
//        const char* s = sout.str().c_str();
//        auto fps_stats_size = ImGui::CalcTextSize(s);
//        ImGui::SameLine(ImGui::GetContentRegionMax().x - fps_stats_size.x);
//        ImGui::Text("%s", s);
//        ImGui::EndMainMenuBar();
//    }
    
    // Hierarchy view
    //ImGui::BeginDock("Hierarchy");
    ImGui::Begin("Hierarchy");
    if (ImGui::Button("Create")) {
        auto go = Scene::CreateGameObject("GameObject");
    }
    
    if (go != nullptr) {
        ImGui::SameLine();
        if (ImGui::Button("Destroy")) {
            Object::Destroy(go);
            //Selection::setActiveGameObject(nullptr);
        }
    }
    m_idCount = 0;
    ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize()); // Increase spacing to differentiate leaves from expanded contents.
    for (auto go : Scene::m_gameObjects) {
        if (go->transform()->parent() == nullptr) {
            HierarchyItem(go);
        }
    }
    ImGui::PopStyleVar();
    //ImGui::EndDock();
    ImGui::End();
    
    
    // Project view
    //ImGui::BeginDock("Project");
    //ImGui::EndDock();
    
//    if (m_showAssectSelectionDialogBox) {
//        ImGui::OpenPopup("Select ...");
//        ImGuiWindowFlags window_flags = 0;
//        window_flags |= ImGuiWindowFlags_NoCollapse;
//        if (ImGui::BeginPopupModal("Select ...", &m_showAssectSelectionDialogBox, window_flags)) {
//            for (auto m : Mesh::m_meshes) {
//                if (ImGui::Button(m->name().c_str())) {
//                    Debug::Log("%s", m->name().c_str());
//                }
//            }
//            ImGui::Separator();
//            ImGui::EndPopup();
//        }
//    }
    
    ImGui::Render();
}

void EditorGUI::Clean()
{
    ImGui_ImplGlfwGL3_Shutdown();
}

//void GUI::OnKey(int key, int action)
//{
//}
//
//void GUI::OnWindowSizeChanged(int width, int height)
//{
//}
//
//void GUI::OnMouse(double xpos, double ypos)
//{
//}
//
//bool GUI::OnMouseButton(int button, int action)
//{
//    return false;
//}
//
//bool GUI::OnMouseScroll(double yoffset)
//{
//    return false;
//}

bool EditorGUI::Button(const char* text)
{
    ImGuiContext& g = *GImGui;
    ImGuiStyle& style = g.Style;
    float w = ImGui::GetWindowWidth() - style.WindowPadding.x * 2.f;
    return ImGui::Button(text, ImVec2(w, 0));
}

void EditorGUI::SelectMeshDialogBox(std::function<void(std::shared_ptr<Mesh>)> callback)
{
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse;
    bool is_open = true;
    if (ImGui::BeginPopupModal("Select ...", &is_open, window_flags)) {
        for (auto m : Mesh::m_meshes) {
            if (ImGui::Button(m->name().c_str())) {
                Debug::Log("%s", m->name().c_str());
                //SetMesh(m);
                callback(m);
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::Separator();
        ImGui::EndPopup();
    }
}


void EditorGUI::HierarchyItem(std::shared_ptr<GameObject> gameObject) {
    ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ((gameObject == Selection::activeGameObject())? ImGuiTreeNodeFlags_Selected : 0);
    
    bool is_leaf = (gameObject->transform()->childCount() == 0);
    if (is_leaf) {// no child
        node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }
    if (!gameObject->activeSelf()) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.20f, 0.20f, 0.20f, 1.00f));
    }
    bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)m_idCount, node_flags, "%s", gameObject->name().c_str());

    if (ImGui::IsItemClicked()) {
        Selection::setActiveGameObject(gameObject);
    }
    // child node
    if (!is_leaf) {
        if (node_open) {
            for (auto t : gameObject->transform()->m_children)
                HierarchyItem(t.lock()->gameObject());
            ImGui::TreePop();
        }
    }
    m_idCount ++;
    if (!gameObject->activeSelf()) {
        ImGui::PopStyleColor();
    }
}


void EditorGUI::DrawSceneGizmo()
{
    int w = EditorRenderSystem::width();
    int h = EditorRenderSystem::height();
    glViewport(0, 0, GLsizei(w*0.1f), GLsizei(h*0.1f));
    
    auto shader = sceneGizmoMaterial->shader();
    shader->Use();
    
//#ifdef GLM_FORCE_LEFT_HANDED
    auto camera_pos = Vector3(0, 0, -5);
//#else
//    auto camera_pos = Vector3(0, 0, 5);
//#endif
    sceneGizmoMaterial->SetVector3("unity_LightPosition", camera_pos.normalized());
    auto camera = Camera::main();
    auto view = Matrix4x4::LookAt(camera_pos, Vector3(0, 0, 0), Vector3(0, 1, 0));
    auto proj = camera->projectionMatrix();
    auto vp = proj * view;
    auto model = Matrix4x4(Quaternion::Inverse(camera->transform()->rotation()));
    
    ShaderUniforms uniforms;
    sceneGizmoMaterial->SetMatrix("MATRIX_MVP", vp*model);
    sceneGizmoMaterial->SetMatrix("MATRIX_IT_MV", view*model);
    sceneGizmoMaterial->SetVector3("_Color", Vector3(1, 1, 1));
    shader->PreRender();
    sceneGizmoMaterial->Update();
    shader->CheckStatus();
    cubeMesh->Render();

    auto s = Matrix4x4::Scale(0.5f, 0.75f, 0.5f);
    
    float f[] = {
        -1,  0,  0,    0, 0, -90,
         0, -1,  0,    0, 0,   0,
         0,  0, -1,   90, 0,   0,
         1,  0,  0,    0, 0,  90,
         0,  1,  0,  180, 0,   0,
         0,  0,  1,  -90, 0,   0,
    };
    static Transform t;
    for (int i = 0; i < 6; ++i) {
        int j = i*6;
        t.setLocalPosition(f[j], f[j+1], f[j+2]);
        t.setLocalEulerAngles(f[j+3], f[j+4], f[j+5]);
        //t.Update();
        auto modelMat = model * t.localToWorldMatrix() * s;
        sceneGizmoMaterial->SetMatrix("MATRIX_MVP", vp*modelMat);
        sceneGizmoMaterial->SetMatrix("MATRIX_IT_MV", (view * modelMat).transpose().inverse());
        if (i >= 3)
            sceneGizmoMaterial->SetVector3("_Color", Vector3(f[j], f[j+1], f[j+2]));
        sceneGizmoMaterial->Update();
        coneMesh->Render();
    }
    
    shader->PostRender();
    
    auto v = camera->viewport();
    glViewport(GLint(w*v.x), GLint(h*v.y), GLsizei(w*v.z), GLsizei(h*v.w));
}


NAMESPACE_FISHEDITOR_END