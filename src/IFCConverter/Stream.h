#pragma once
#include "../ifcgeom/IfcGeomIterator.h"
#include <fstream>

inline bool equals(double x, double y, double epsilon)
{
	return fabs(x - y) < epsilon;
}

struct Color
{
	double r = 0.0, g = 0.0, b = 0.0;

	bool operator == (const Color& rhs) const
	{
		return equals(r, rhs.r, 0.001) && equals(g, rhs.g, 0.001) && equals(b, rhs.b, 0.001);
	}
};

class Stream 
{
public:
	Stream(const std::string name) : name(name) {};
	virtual ~Stream() = default;
	virtual void writeMesh(const std::string& meshName, const std::vector<double>& vertices, const std::vector<double>& normals, const std::vector<int>& indices, const std::vector<IfcGeom::Material>& materials) {}
	virtual void close() {}
protected:
    std::string parseFileName(const std::string& filePath)
    {
        std::string str = filePath;
        const int lastSlash = str.find_last_of("\\/");
        if(std::string::npos != lastSlash)
        {
            str.erase(0, lastSlash + 1);
        }
        return str;
    }
	std::string name;
};