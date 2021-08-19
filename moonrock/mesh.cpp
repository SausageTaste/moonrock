#include "mesh.h"


namespace moonrock {

    void gen_mesh_quad(std::vector<VertexStatic>& output, const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3) {
        output.push_back(VertexStatic{ p0, glm::vec2{0, 0} });
        output.push_back(VertexStatic{ p1, glm::vec2{0, 1} });
        output.push_back(VertexStatic{ p2, glm::vec2{1, 1} });

        output.push_back(VertexStatic{ p0, glm::vec2{0, 0} });
        output.push_back(VertexStatic{ p2, glm::vec2{1, 1} });
        output.push_back(VertexStatic{ p3, glm::vec2{1, 0} });
    }

}
