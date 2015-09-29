#include <gp_Quaternion.hxx>
#include "../cJSON/cJSON.h"
#include "../ifcparse/IfcFile.h"
#include "../IFCConverter/EnumClassDeclaration.h"
#include "../IFCConverter/ObjStream.h"
#include "../IFCConverter/StlStream.h"

static const double PI = 3.1415926535897932384626433832795;
static const double DEGTORAD = PI / 180.0;

//IfcGeom::IteratorSettings settings
// ApplyDefaultMaterials:
//     Applies default materials to entity instances without a surface style.
//UseWorldCoords:
//     Specifies whether to apply the local placements of building elements
//     directly to the coordinates of the representation mesh rather than
//     to represent the local placement in the 4x3 matrix, which will in that
//     case be the identity matrix.
//WeldVertices:
//     Specifies whether vertices are welded, meaning that the coordinates
//     vector will only contain unique xyz-triplets. This results in a 
//     manifold mesh which is useful for modelling applications, but might 
//     result in unwanted shading artifacts in rendering applications.
//     When WeldVertices is enabled normals are not generated.
//SewShells:
//     Specifies whether to sew IfcConnectedFaceSets (open and closed shells) to
//     TopoDS_Shells or whether to keep them as a loose collection of faces.
//ConvertBackUnits:
//     Internally IfcOpenShell measures everything in meters. This settings
//     specifies whether to convert IfcGeomObjects back to the units in which
//     the geometry in the IFC file is specified.
//FasterBooleans:
//     Specifies whether to compose IfcOpeningElements into a single compound
//     in order to speed up the processing of opening subtractions.
//DisableOpeningSubTractions:
//     Disables the subtraction of IfcOpeningElement representations from
//     the related building element representations.
//ExcludeSolidsAndSurfaces:
//    Specifies whether to exclude subtypes of IfcSolidModel and IfcSurface.
DECLARE_ENUM_CLASS(GeometryOperations, ApplyDefaultMaterials, UseWorldCoords, WeldVertices, SewShells, ConvertBackUnits, FasterBooleans, DisableOpeningSubTractions, ExcludeSolidsAndSurfaces);
DECLARE_ENUM_CLASS(ConversionType, obj, stl);

struct ConversionData {
	char *input, *ouput;
	std::vector<ConversionType> conversionTypes;
	std::vector<GeometryOperations> geometryOperations;
	std::set<std::string> excludeEntities;
	std::set<std::string> includeEntities;
};

static const int iteratorSettings[] =
{
	IfcGeom::IteratorSettings::APPLY_DEFAULT_MATERIALS,
	IfcGeom::IteratorSettings::USE_WORLD_COORDS,
	IfcGeom::IteratorSettings::WELD_VERTICES,
	IfcGeom::IteratorSettings::SEW_SHELLS,
	IfcGeom::IteratorSettings::CONVERT_BACK_UNITS,
	IfcGeom::IteratorSettings::FASTER_BOOLEANS,
	IfcGeom::IteratorSettings::DISABLE_OPENING_SUBTRACTIONS,
	IfcGeom::IteratorSettings::EXCLUDE_SOLIDS_AND_SURFACES
};

const int numArgs = 4;

void printUsage() {
	std::cout << "Usage: IFCConverter command line parameters:" << std::endl
		<< "1. Input ifc file:" << std::endl
		<< "2. Name of the output file, without file extension" << std::endl
		<< "3. config file" << std::endl
		<< std::endl;
}

bool parseConfigFile(char* fileName, ConversionData& conversionData)
{
	if (!fileName)
		return false;

	std::ifstream ifs(fileName);
	bool success = true;

	if(ifs)
	{
		std::string fileContent((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
		cJSON* cJSONroot = cJSON_Parse(fileContent.c_str());
		if(!cJSONroot)
		{
			std::cout << "Failed to parse config file. Error before: " << cJSON_GetErrorPtr() << std::endl;
			success = false;
		}
		else
		{
			cJSON* conversionTypes = cJSON_GetObjectItem(cJSONroot, "conversionTypes");
			cJSON* geometryOperations = cJSON_GetObjectItem(cJSONroot, "geometryOperations");
			cJSON* exclude = cJSON_GetObjectItem(cJSONroot, "exclude");
			cJSON* include = cJSON_GetObjectItem(cJSONroot, "include");

			if(conversionTypes)
			{
				for(int i = 0; i < cJSON_GetArraySize(conversionTypes); i++)
				{
					conversionData.conversionTypes.push_back(enumFromString<ConversionType>(cJSON_GetArrayItem(conversionTypes, i)->valuestring));
				}
			}
			if(geometryOperations)
			{
				for(int i = 0; i < cJSON_GetArraySize(geometryOperations); i++)
				{
					conversionData.geometryOperations.push_back(enumFromString<GeometryOperations>(cJSON_GetArrayItem(geometryOperations, i)->valuestring));
				}
			}
			if (exclude)
			{
				for (int i = 0; i < cJSON_GetArraySize(exclude); i++)
				{
					conversionData.excludeEntities.insert(cJSON_GetArrayItem(exclude, i)->valuestring);
				}
			}
			if (include)
			{
				for (int i = 0; i < cJSON_GetArraySize(include); i++)
				{
					conversionData.excludeEntities.insert(cJSON_GetArrayItem(include, i)->valuestring);
				}
			}
		}
		cJSON_Delete(cJSONroot);
	}
	else
	{
		std::cout << "Failed to open config file " << fileName << std::endl;
		success = false;
	}

	return success;
}

bool parseArgs(int argc, char** argv, ConversionData& conversionData)
{
	bool succeed = false;
	if(argc == numArgs)
	{
		conversionData.input = argv[1];
		conversionData.ouput = argv[2];

		succeed = parseConfigFile( argv[3], conversionData);
	}
	else
	{
		printUsage();
	}

	return succeed;
}

void convert(const ConversionData& conversionData)
{
	IfcGeom::IteratorSettings settings;

	//WELD_VERTICES default value is true, but then normals are not generated.
	//Disable it by default. If one want to weld vertices, add WeldVertices in geometryOperations in config file. 
	settings.set(IfcGeom::IteratorSettings::WELD_VERTICES, false);

	for (auto operation : conversionData.geometryOperations)
	{
		settings.set(iteratorSettings[static_cast<int>(operation)], true);
	}


	IfcGeom::Iterator<double> geomIterator(settings, conversionData.input);

	if (!conversionData.excludeEntities.empty())
	{
		geomIterator.excludeEntities(conversionData.excludeEntities);
	}

	if (!conversionData.includeEntities.empty())
	{
		geomIterator.includeEntities(conversionData.includeEntities);
	}


	if (!geomIterator.initialize())
	{
		std::cout << "Unable to parse " << conversionData.input << " file" << std::endl;
		return;
	}

	//Create conversion streams.
	std::vector<Stream*> conversionStreams;
	for (auto type : conversionData.conversionTypes)
	{
		switch (type)
		{
			case ConversionType::obj:
			{
				conversionStreams.push_back(new ObjStream(std::string(conversionData.ouput)));
				break;
			}
			case ConversionType::stl:
				conversionStreams.push_back(new StlStream(std::string(conversionData.ouput)));
				break;
		}
	}

	//Create rotation that will rotate model so that y-axis is up-axis.
	gp_Quaternion rotZ(gp_Vec(0.0, 0.0, 1.0), 90.0 * DEGTORAD);
	gp_Quaternion rotY(gp_Vec(0.0, 1.0, 0.0), 90.0 * DEGTORAD);
	gp_Quaternion rotFinal = rotZ * rotY;

	do
	{
		auto triangulationElement = static_cast<const IfcGeom::TriangulationElement<double>*>(geomIterator.get());
		const IfcGeom::Representation::Triangulation<double>& mesh = triangulationElement->geometry();

		const std::vector<double>& vertices = mesh.verts();
		const std::vector<double>& normals = mesh.normals();
		std::vector<double> rotatedVertices(vertices.size());
		std::vector<double> rotatedNormals(normals.size());

		for (unsigned int i = 0; i < vertices.size() / 3; ++i)
		{
			gp_Vec ver(vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2]);
			auto v = rotFinal.Multiply(ver.XYZ());
			rotatedVertices[i * 3] = v.X();
			rotatedVertices[i * 3 + 1] = v.Y();
			rotatedVertices[i * 3 + 2] = v.Z();
		}

		for (unsigned int i = 0; i < normals.size() / 3; ++i)
		{
			gp_Vec nor(normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]);
			auto n = rotFinal.Multiply(nor.XYZ());
			rotatedNormals[i * 3] = n.X();
			rotatedNormals[i * 3 + 1] = n.Y();
			rotatedNormals[i * 3 + 2] = n.Z();
		}

		//Write mesh data into the streams.
		for (auto &stream : conversionStreams)
		{
			stream->writeMesh(triangulationElement->name(), rotatedVertices, rotatedNormals, mesh.faces(), mesh.materials());
		}

	} while (geomIterator.next());

	for (auto &stream : conversionStreams)
	{
		delete stream;
	}
}

int main(int argc, char** argv) {
   
	ConversionData conversionData;
	bool succeed = parseArgs(argc, argv, conversionData);

	if (!succeed)
	{
		std::cout << "Failed to parse commandline options " << std::endl;
		return 1;
	}

	convert(conversionData);

	return 0;
}