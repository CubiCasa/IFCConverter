#include "../IFCConverter/StlStream.h"


StlStream::StlStream(const std::string name) :
Stream(name)
{
    std::string geometryFileName = name + std::string(".stl");
    geometryStream.open(geometryFileName, std::ofstream::out);
    geometryStream << "solid " << parseFileName(name) << std::endl;

}

StlStream::~StlStream()
{
    close();
}

void StlStream::close()
{
    geometryStream << "endsolid " << parseFileName(name) << std::endl;
    geometryStream.close();
}

void StlStream::writeMesh(const std::string& meshName, const std::vector<double>& vertices, const std::vector<double>& normals, const std::vector<int>& indices, const std::vector<IfcGeom::Material>& materials)
{
    for(unsigned int i = 0; i < indices.size() / 3; ++i)
    {
        const int v1 = indices[i * 3];
        const int v2 = indices[i * 3 + 1];
        const int v3 = indices[i * 3 + 2];

        //Get per-face normal
        std::array<double, 3> n1 = { normals[v1 * 3], normals[v1 * 3 + 1], normals[v1 * 3 + 2] };
        std::array<double, 3> n2 = { normals[v2 * 3], normals[v2 * 3 + 1], normals[v2 * 3 + 2] };
        std::array<double, 3> n3 = { normals[v3 * 3], normals[v3 * 3 + 1], normals[v3 * 3 + 2] };
        std::array<double, 3> fNor = getFacetNormal(n1, n2, n3);
        geometryStream << " facet normal " << fNor[0] << " " << fNor[1] << " " << fNor[2] << std::endl;
        geometryStream << "  outer loop" << std::endl;

        geometryStream << "  vertex " << vertices[v1 * 3] << " " << vertices[v1 * 3 + 1] << " " << vertices[v1 * 3 + 2] << std::endl;
        geometryStream << "  vertex " << vertices[v2 * 3] << " " << vertices[v2 * 3 + 1] << " " << vertices[v2 * 3 + 2] << std::endl;
        geometryStream << "  vertex " << vertices[v3 * 3] << " " << vertices[v3 * 3 + 1] << " " << vertices[v3 * 3 + 2] << std::endl;

        geometryStream << "  endloop" << std::endl;
        geometryStream << " endfacet" << std::endl << std::endl;
    }
}

std::array<double, 3> StlStream::getFacetNormal(const std::array<double, 3>& n1, const std::array<double, 3>& n2, const std::array<double, 3>& n3)
{
    std::array<double, 3> normal;
    normal[0] = n1[0] + n2[0] + n3[0];
    normal[1] = n1[1] + n2[1] + n3[1];
    normal[2] = n1[2] + n2[2] + n3[2];
    double len = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
    double invLen = 1.0 / len;
    normal[0] *= invLen;
    normal[1] *= invLen;
    normal[2] *= invLen;

    return normal;
}