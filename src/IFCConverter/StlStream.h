#pragma once

#include "../IFCConverter/Stream.h"

class StlStream : public Stream
{
public:
	StlStream(const std::string name);
	~StlStream();
	void close() override;
	void writeMesh(const std::string& meshName, const std::vector<double>& vertices, const std::vector<double>& normals, const std::vector<int>& indices, const std::vector<IfcGeom::Material>& materials) override;
private:
	std::ofstream geometryStream;
	std::array<double, 3> getFacetNormal(const std::array<double, 3>& n1, const std::array<double, 3>& n2, const std::array<double, 3>& n3);
};