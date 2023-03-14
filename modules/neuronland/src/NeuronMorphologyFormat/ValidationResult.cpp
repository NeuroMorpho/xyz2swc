//
//
//

#include "ValidationResult.h"


const char* sg_validationResultStrings[] = 
{
	"False",
	"Unlikely",
	"Likely",
	"True",
	"Undeterminable"
};

const char* ValidationResultToString(ValidationResult result)
{
	return sg_validationResultStrings[result];
}
