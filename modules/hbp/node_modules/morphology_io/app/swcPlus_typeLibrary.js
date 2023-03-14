var swcPlus_version = 0.3

var swcPlus_maturity = "alpha"

// maybe remove this
var swcPlus_typeId = {
  "undefined":0,
  "soma":1,
  "axon":2,
  "dendrite":3,
  "apical":4
}

// * Every attribute is assigned a two-member array with default value and attribute-type.
//   If default is null, the attribute is required.
//   If attribute-type is null, the attribute is fixed.
//
// * Attributes starting with _ are private attributes.
//
// * Every XML-element in swcPlus that has no child-elements defined
//   may contain free text between its opening and closing tags.
//   This text is treated as comment text.
//
// * In the schema below, keys that have dictionary values (curly 
//   braces) are xml elements, keys with array values (square brackets) 
//   represent attributes
//
var swcPlus_schema = {
  "swcPlus": {
    "version":[swcPlus_version,"versionType"],
    "customTypes": {
      "base":{
        "id":[  ,"idType"],
        "name":[null,"nameType"],
        "group":["","groupType"],
        "geometry":["tree","geomType"],
        "cellPart":["","partType"],
        "_extends":"",
        "_docstring":"Base Type that provides attributes that all Types have."
      },
      "tree":{
        "geometry":["tree",null],
        "_extends":"base",
        "_docstring":"Tree of connected points <tt>(x,y,z)</tt> with radius <tt>r</tt>, as used in regular SWC files"
      },
      "border":{
        "group":["borders","groupType"],
        "geometry":["border",null],
        "_extends":"base",
        "_docstring":"Border, formed by a non-branching list of connected points <tt>(x,y,z)</tt>. Often used for tracing borders between cortical layers or cortical regions. <tt>r</tt> represents the wall thickness of the border in micrometers."
      },
      "contour":{
        "group":["contours","groupType"],
        "fill":["","fillType"],
        "geometry":["contour",null],
        "_extends":"base",
        "_docstring":"Contour, formed by a non-branching list of connected points <tt>(x,y,z)</tt>. Often used for tracing the cell body. <tt>r</tt> represents the wall thickness of the contour in micrometers."
      },
      "marker":{
        "group":["markers","groupType"],
        "symbol":["o","symbolType"],
        "geometry":["marker",null],
        "_extends":"base",
        "_docstring":"Marker located at the point <tt>(x,y,z)</tt>. <tt>r</tt> represents the marker size in micrometers."
      },
      "image":{
        "group":["images","groupType"],
        "src":[null,"imageSourceType"],
        "geometry":["image",null],
        "_extends":"base",
        "_docstring":"Image anchor. The image is treated as a volume with only a single point in the y-dimension. Coordinate transformations are the same as for the volume anchor."
      },
      "volume":{
        "group":["volumes","groupType"],
        "src":[null,"volumeSourceType"],
        "geometry":["volume",null],
        "_extends":"base",
        "_docstring":"Volume anchor. If the volume is referred to by a single point, then <tt>(x,y,z)</tt> is used to translate the volume, and <tt>r</tt> to scale the volume. If the volume is referred to by a chain of four points, then the <tt>(x,y,z,r)</tt> of these four points are used as columns of a 4x4 transformation+translation matrix that is to be applied to the volume."
      },
      "surface":{
        "group":["surfaces","groupType"],
        "src":[null,"surfaceSourceType"],
        "geometry":["surface",null],
        "_extends":"base",
        "_docstring":"Surface anchor. Vertices and faces are specified in the file referenced by the src-attribute. Coordinate transformations of vertices are the same as for the volume anchor."
      },
      "undefined":{
        "id":[0,null],
        "name":["undefined",null],
        "_extends":"base",
        "_docstring":"Use of the <i>Undefined</i> Type is discouraged. It causes the Type of a point to be inherited from its parent point. Corresponds to Type 0 in regular <tt>SWC</tt>."
      },
      "soma":{
        "id":[1,null],
        "name":["Soma",null],
        "cellPart":["soma",null],
        "_extends":"tree",
        "_docstring":"Soma (=cell body), in its representation as a <tt>Tree</tt>. Corresponds to Type 1 in regular <tt>SWC</tt>."
      },
      "axon":{
        "id":[2,null],
        "name":["Axon",null],
        "cellPart":["axon",null],
        "group":["axons","groupType"],
        "_extends":"tree",
        "_docstring":"Axon. Corresponds to Type 2 in regular <tt>SWC</tt>."
      },
      "dendrite":{
        "id":[3,null],
        "name":["(basal) Dendrite",null],
        "cellPart":["dendrite",null],
        "group":["dendrites","groupType"],
        "_extends":"tree",
        "_docstring":"(basal) Dendrite. Corresponds to Type 3 in regular <tt>SWC</tt>."
      },
      "apical":{
        "id":[4,null],
        "name":["Apical dendrite",null],
        "cellPart":["apical dendrite",null],
        "group":["dendrites","groupType"],
        "_extends":"tree",
        "_docstring":"Apical dendrite. Corresponds to Type 4 in regular <tt>SWC</tt>."
      },
      "unknown":{
        "_extends":"base",
        "_docstring":"Unknown type. Indicates failure to assign a proper type to a point."
      },
      "somaContour":{
        "name":["Soma (contour)",null],
        "cellPart":["soma",null],
        "group": ["soma contours","groupType"],
        "_extends":"contour",
        "_docstring":"Soma (=cell body), in its representation as a <tt>Contour</tt>."
      },
      "layerBorder":{
        "name":["Border between layers {atlas:layerA} and {atlas:layerB}",null],
        "group":["layer borders","groupType"],
        "atlas:layerA":[null,"atlas:layerType"],
        "atlas:layerB":[null,"atlas:layerType"],
        "_extends":"border",
        "_docstring":"Border between two layers <tt>a</tt> and <tt>b</tt> (<tt>b</tt> larger than <tt>a</tt>)."
      },
      "regionContour": {
        "name":["Region {atlas:region} contour",null],
        "group":["region contours","groupType"],
        "atlas:region":[null,"atlas:regionType"],
        "_extends":"contour",
        "_docstring":"Contour of the brain region specified in <tt>atlas:a</tt>. The <tt>atlas</tt> namespace must be declared in the swcPlus element."
      },
      "regionBorder": {
        "name":["Region {atlas:regionA}|{atlas:regionB} border",null],
        "group":["region borders","groupType"],
        "atlas:regionA":[null,"atlas:regionType"],
        "atlas:regionB":[null,"atlas:regionType"],
        "_extends":"border",
        "_docstring":"Border between the brain regions specified in <tt>atlas:regionA</tt> and <tt>atlas:regionB</tt>. The <tt>atlas</tt> namespace must be declared in the swcPlus element."
      },
      "sectionContour": {
        "name":["Slice level {level}",null],
        "group":["region borders","groupType"],
        "level":[null,"levelType"],
        "_extends":"contour",
        "_docstring":"Outer contour of one of the brain sections from which the neuron was reconstructed."
      },
      "spine": {
        "name":["Spine","nameType"],
        "cellPart":["spine"],
        "group":["spines","groupType"],
        "_extends":"marker",
        "_docstring":"Marker that represents a spine."
      }
    },
    "customProperties": {
      "for": {
        "points":["","csvType"],
        "objects":["","csvType"],
        "_choice": {
          "set":[1,"unbounded"]
        }
      },
      "set": {
        "*":["","stringType"],
        "json:*":["null","jsonType"],
        "csv:*":["","csvType"]
      }
    },
    "metaData": {
      "originalHeader": {
        "_docstring":"The original free-text SWC header (if any) goes here."
      },
      "fileHistory": {
        "originalName":["","stringType"],
        "originalFormat":["","formatType"],
        "_choice": {
          "modification":[0,"unbounded"]
        },
        "_docstring":"List of modifications done to get from the original file to this version."
      },
      "modification": {
        "date":["","dateType"],
        "software":["","softwareType"],
        "command":["","commandType"],
        "summary":["","summaryType"],
        "_docstring":"Modification applied to the previous version of the data. Use software=\"text editor\" if done manually."
      },
      "project": {
        "id":["","stringType"],
        "name":"stringType",
        "url":"urlType",
        "consortium":"stringType",
        "_choice": {
          "grant":[0,10]
        },
        "_docstring":"(research) Project in the context of which this data was collected."
      },
      "grant":{
        "id":["","stringType"],
        "name":["","stringType"],
        "url":["","urlType"],
        "issuer":["","stringType"],
        "_docstring":"Grant that pays for the project (multiple allowed)."
      },
      "researchTeam": {
        "_choice":{
          "contributor":[0,100]
        }
      },
      "experiment":{
        "_choice": {
          "protocol":[0,1],
          "staining":[0,10],
          "slice":[0,100],
          "microscope":[0,1]
        },
        "_docstring":"Experiment that produced the data."
      },
      "contributor":{
        "firstName":["","stringType"],
        "lastName":["","stringType"],
        "role":["","roleType"],
        "affiliation":["","stringType"],
        "_docstring":"Contributor to the experiment, reconstruction or grant proposal."
      },
      "protocol":{
        "id":["","stringType"],
        "name":["","stringType"],
        "preparation":["?","preparationType"],
        "treatment":["?","treatmentType"],
        "_docstring":"Experimental protocol summary."
      },
      "staining":{
        "id":[""],
        "method":["","stainingType"],
        "substance":["","substanceType"],
        "_docstring":"Slice or tissue staining properties."
      },
      "slice":{
        "orientation":["","orientationType"],
        "thickness":["","thicknessType"],
        "shrinkage":["","shrinkageType"],
        "_docstring":"Slice properties."
       },
      "microscope":{
        "magnification":["","magnificationType"],
        "objective":["","objectiveType"],
        "_docstring":"Microscope properties."
      },
      "reconstruction":{
        "software":["","softwareType"],
        "tracingMode":["","tracingModeType"],
        "quality":["","reconstructionQualityType"],
        "_choice":{
          "soma":[1,1],
          "axon":[1,1],
          "dendrites":[1,1]
        },
        "_docstring":"Reconstruction parameters and completeness."
      },
      "soma":{
        "representation":["","somaRepresentationType"],
        "_docstring":"Soma representation in the original file."
      },
      "axon":{
        "incomplete":["","boolType"],
        "reason":["","incompleteReasonType"],
        "quality":["","qualityType"],
        "_docstring":"Whether and why the axon is incomplete."
      },
      "dendrites":{
        "incomplete":["","boolType"],
        "reason":["","incompleteReasonType"],
        "quality":["","qualityType"],
        "_docstring":"Whether and why the dendrite is incomplete."
      },
      "dataSharing":{
        "_choice": {
          "repository":[0,100],
          "license":[0,10],
          "publication":[0,"unbounded"]
        },
        "_docstring":"Data sharing aspects."
      },
      "repository":{
        "name":["","repositoryType"],
        "dataDoi":["","dataDoiType"],
        "dataUrl":["","dataUrlType"],
        "_docstring":"Data sharing repository properties."
      },
      "license":{
        "id":["","stringType"],
        "name":["","nameType"],
        "url":["","urlType"],
        "_docstring":"License that governs the use of this data."
      },
      "publication":{
        "name":["","stringType"],
        "doi":["","doiType"],
        "url":["","urlType"],
        "_docstring":"Publication in which this data is introduced. May play a role in the citation policy that comes with the license."
      },
      "specimen":{
        "species":["","speciesType"],
        "strain":["","strainType"],
        "age":["","ageType"],
        "gender":["","genderType"],
        "_docstring":"Specimen properties"
      },
      "cell":{
        "type":["","cellType"],
        "atlas:region":["","atlas:regionType"],
        "layer":["","layerType"],
        "_docstring":"Cell type and location properties"
      }
    },
    "swcPoints":{
      "type":["","pointsType"],
      "_docstring":"The SWC 7-column point data, with id|type|x,y,z|r|parent"
    }
  }
}

var swcPlus_attributeTypes = {
  "atlas:regionType":{
    "_docstring":"Specifies a brain region abbreviation, which must be part of the <tt>atlas</tt> namespace declared in the swcPlus element."
  },
  "colorType":{
    "restriction":{
      "pattern":"#([\\dA-F]{3}|[\\dA-F]{4}|[\\dA-F]{6}|[\\dA-F]{8})"
    },
    "_docstring":"Suggests a color to use when drawing the object. Specify as #RGB or #RRGGBB or #RGBA or #RRGGBBAA where R,G,B,A are hex-values for red, green and blue and alpha (transparency)"
  },
  "fillType":{
    "restriction":{
      "pattern":"#([\\dA-F]{3}|[\\dA-F]{4}|[\\dA-F]{6}|[\\dA-F]{8})"
    },
    "_docstring":"Represents the fill colour of a contour. Specify as #RGB or #RRGGBB or #RGBA or #RRGGBBAA where R,G,B,A are hex-values for red, green and blue and alpha (transparency)."
  },
  "geomType":{
    "restriction": {
      "enumeration":["tree","marker","property","border","contour","image","volume","surface"]
    },
    "_docstring":"Geometrical primitive used by the object, see also the Types with the corresponding names."
  },
  "groupType":{
    "_docstring":"When displaying the object-tree of an <tt>SWC+</tt> file, all objects of this Type will be grouped under <tt>group</tt>, this is useful when many objects of the same Type are expected."
  },
  "idType":{
    "simpleType":"xs:integer",
    "restriction":{
      "minInclusive":"16",
      "maxInclusive":"65535"
    },
    "_docstring":"Corresponds to the <tt>TypeID</tt> used in the second column of the <i>points matrix</i>. For custom Types, it must be between 16 and 65535."
  },
  "imageSourceType":{
    "restriction": {
      "pattern":".*\\.(png|PNG|jpg|JPG|jpeg|JPEG)"
    },
    "_docstring":"Source URL of the image file, in PNG or JPEG format."
  },
  "layerType":{
    "restriction": {
      "enumeration":["1","2","3","4","5","6","3A","3B","4A","4B","4C","5A","5B","4C-alpha","4C-beta","4C-gamma"]
    },
    "_docstring":"Valid layer values are 1,2,3,4,5,6, 3A,3B, 4A,4B,4C, 5A,5B, 4C-alpha,4C-beta,4C-gamma."    
  },
  "linkedSourceType":{
    "restriction": {
      "pattern":".*\\.(swc|SWC)"
    },
    "_docstring":"Source URL of the linked file, in <tt>SWC+</tt> format."
  },
  "nameType":{
    "_docstring":"Name of the object. Multiple objects of the same name may exist. May contain expressions between curly brackets, like \"Layer {a}\". The value of attribute <tt>a</tt> will be substituted for the \"{a}\" part of <tt>name</tt>."
  },
  "surfaceSourceType":{
    "restriction": {
      "pattern":".*\\.(obj|OBJ)"
    },
    "_docstring":"Source URL of the volume file, in <a href=\"https://en.wikipedia.org/wiki/Wavefront_.obj_file\">WaveFront OBJ format</a>."
  },
  "symbolType":{
    "_docstring":"Marker symbol character."
  },
  "partType":{
    "_docstring":"Cell or tissue part that this type represents."
  },
  "volumeSourceType":{
    "restriction": {
      "pattern":".*\\.(nrrd|NRRD)"
    },
    "_docstring":"Source URL of the surface file, in <a href=\"https://en.wikipedia.org/wiki/Nrrd\">NRRD format</a>."
  }
}

if (typeof module !== 'undefined' && module.exports) {
  module.exports = { version:swcPlus_version,typeId:swcPlus_typeId,schema:swcPlus_schema }
}