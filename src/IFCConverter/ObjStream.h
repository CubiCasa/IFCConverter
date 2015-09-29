#pragma once

#include "../IFCConverter/Stream.h"

struct ObjMaterial
{
	Color Kd;
	Color Ks;
	double Ns = 0.0, Tr = 0.0;
	std::string name;
	//Checks if the material parameters are same, doesn't compare names.
	bool operator == (const ObjMaterial& rhs) const
	{
		return Kd == rhs.Kd && Ks == rhs.Ks && equals(Ns, rhs.Ns, 0.001) && equals(Tr, rhs.Tr, 0.001);
	}

	ObjMaterial() {}
	ObjMaterial(const IfcGeom::Material& material)
	{
		const double* diffuse = material.diffuse();
		const double* specular = material.specular();
		Kd.r = diffuse[0], Kd.g = diffuse[1], Kd.b = diffuse[2];
		Ks.r = specular[0], Ks.g = specular[1], Ks.b = specular[2];
		Ns = material.specularity();
		Tr = 1.0 - material.transparency();
		name = material.name();
	}
};

class ObjStream : public Stream
{
public:
	ObjStream(const std::string name);
	~ObjStream();
	void close() override;
	void writeMesh(const std::string& meshName, const std::vector<double>& vertices, const std::vector<double>& normals, const std::vector<int>& indices, const std::vector<IfcGeom::Material>& materials) override;
private:
	int vertexCount = 1;
	std::ofstream geometryStream;
	std::ofstream materialStream;
	std::vector<ObjMaterial> createdMaterials;
};
