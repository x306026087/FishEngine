#ifndef MeshRenderer_hpp
#define MeshRenderer_hpp

#include "Renderer.hpp"

namespace FishEngine
{
    class MeshRenderer : public Renderer
    {
    public:
        InjectClassName(MeshRenderer);

        MeshRenderer() = default;

        MeshRenderer(PMaterial material);

        //protected:
        virtual void Render() const override;
        
        virtual Bounds localBounds() const override;
        virtual void OnDrawGizmosSelected() override;

    private:
        friend class FishEditor::EditorGUI;
    };
}

#endif /* MeshRenderer_hpp */