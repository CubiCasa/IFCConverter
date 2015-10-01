# IFCConverter

IFCConverter is a software for converting ifc files into commonly used 3D formats. It is implemented on top of ifcparse and ifcgeom libraries from ifcOpenShell project.

http://ifcopenshell.org

Currently supported output formats:

* obj
* stl (ascii)

##Usage

IFCConverter is a commandline tool and it needs the following input parameters:

1. ifc file name.
2. name of the output file (without extension).
3. json config file.

Example config file.

```javascript
{
  "conversionTypes" : ["obj", "stl"],
  "geometryOperations" : ["UseWorldCoords", "SewShells"],
  "exclude" :["ifcopeningelement", "ifcspace"]
}
```

**conversionTypes** specifies the output formats. For example ["obj", "stl"] will convert a single ifc file into both formats in the same run. If you want to convert an ifc file into one format (obj), use **conversionTypes**  ["obj"]

**geometryOperations** specifies the behavior of the converter. These are options for ifcgeom IteratorSettings. Descriptions taken from ifcOpenShell page.

| Value        | Description           |
| ------------- |:-------------|
| ApplyDefaultMaterials      | Applies default materials to entity instances without a surface style|
| UseWorldCoords      | Specifies whether to apply the local placements of building elements directly to the coordinates of the representation mesh rather than to represent the local placement in the 4x3 matrix, which will in that case be the identity matrix     |
| WeldVertices | Specifies whether vertices are welded, meaning that the coordinates vector will only contain unique xyz-triplets. This results in a  manifold mesh which is useful for modelling applications, but might result in unwanted shading artifacts in rendering applications. When WeldVertices is enabled normals are not generated.|
| SewShells | Specifies whether to sew IfcConnectedFaceSets (open and closed shells) to TopoDS_Shells or whether to keep them as a loose collection of faces. |
| ConvertBackUnits | Internally IfcOpenShell measures everything in meters. This settings specifies whether to convert IfcGeomObjects back to the units in which the geometry in the IFC file is specified. |
| FasterBooleans | Specifies whether to compose IfcOpeningElements into a single compound in order to speed up the processing of opening subtractions. |
| DisableOpeningSubTractions |  Disables the subtraction of IfcOpeningElement representations from the related building element representations. |
| ExcludeSolidsAndSurfaces | Specifies whether to exclude subtypes of IfcSolidModel and IfcSurface. |

**exclude** specifies a list of ifc elements which are excluded from the conversion process.

##Dependencies

* [Boost](http://www.boost.org/)
* Open Cascade
  [Official](http://www.opencascade.org/getocc/download/loadocc/) or [community edition](https://github.com/tpaviot/oce)  
  For converting IFC representation items into BRep solids and tessellated meshes
* [ICU](http://site.icu-project.org/) *optional*  
  For handling codepages and Unicode in the parser

##Compiling

###Compiling on Windows

Users are advised to use the Visual Studio .sln file in the win/ folder. 
Visual Studio solutions expects that the system has two environment variables:

**BOOST_ROOT** : path to the Boost folder.

**OCE_ROOT** : path to the Open Cascade folder.

###Compiling on *nix

Users are advised to use the cmake file provided in
the cmake/ folder. There might be an Open CASCADE package in your operating
system's software repository. If not, you will need to compile Open 
CASCADE yourself. See http://opencascade.org.

Additionally, on Ubuntu (and possibly other linux flavors) the following steps
install some of the prerequisites:

    $ sudo apt-get install git cmake gcc g++ libftgl-dev libtbb2 libtbb-dev libboost-all-dev libgl1-mesa-dev libfreetype6-dev
    $ git clone https://github.com/tpaviot/oce.git
    $ cd oce
    $ mkdir build && cd build
    $ cmake ..
    $ make
    $ sudo make install   

Building IFCConverter:

    $ cd /path/to/IFCConverter/cmake
    $ mkdir build
    $ cd build
    Optionally:
        $ OCC_INCLUDE_DIR="/path/to/OpenCASCADE/include"
        $ OCC_LIBRARY_DIR="/path/to/OpenCASCADE/lib"
        $ export OCC_INCLUDE_DIR
        $ export OCC_LIBRARY_DIR
    $ cmake ../
    $ make

