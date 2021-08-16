#include "mesh.h"


namespace moonrock {

    void gen_mesh_quad(std::vector<Vertex>& output, const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3) {
        output.push_back(Vertex{p0});
        output.push_back(Vertex{p1});
        output.push_back(Vertex{p2});

        output.push_back(Vertex{p0});
        output.push_back(Vertex{p2});
        output.push_back(Vertex{p3});
    }

}
