
#ifndef MODEL_VIEWER_MATERIAL_H
#define MODEL_VIEWER_MATERIAL_H

#include "glm/vec3.hpp"

namespace modelViewer::res {
    
    class material_property
    {
    protected:
        std::string m_Name;
    public:
        explicit material_property(std::string& name)
        {
            m_Name = name;
        }
        
        std::string  getName()
        {
            return m_Name;
        }
    };
    
    template <typename T>
    class material_property_t : material_property
    {
       
    private:
        T m_Value;
    public:
        explicit material_property_t(std::string name ) : material_property(name)
        {
            
        }
        
        void setValue(T value)
        {
            m_Value = value;
        }
        
        void getValue()
        {
            return m_Value;
        }
    };
    
    class material {
        glm::vec3 diffuseAlbedo{1, 1, 1};
        glm::vec3 specularAlbedo{1, 1, 1};
        float specularIntensity = 128;
        std::vector<std::shared_ptr<material_property>> m_Properties;
    protected:
        void addProperty(std::shared_ptr<material_property> materialProperty)
        {
            m_Properties.push_back(materialProperty);
        }
    };
    
    class material_phong : material
    {
    public:
        material_phong()
        {
            auto specularShininess = std::make_shared<material_property_t<float>>("u_shininess");
            specularShininess->setValue(128);
        }
    };
}

#endif 
