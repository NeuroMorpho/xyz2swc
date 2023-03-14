#ifndef VALIDATIONRESULT_HPP_
#define VALIDATIONRESULT_HPP_

#include "Core/String.hpp"

enum ValidationResult
{
	kValidationFalse,
	kValidationUnlikely,
	kValidationLikely,
	kValidationTrue,

	kUndeterminable
};

const char* ValidationResultToString(ValidationResult result);

#endif // VALIDATIONRESULT_HPP_

