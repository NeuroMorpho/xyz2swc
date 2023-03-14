#ifndef __HDF5_UTILS_H__
#define __HDF5_UTILS_H__

// Manoj Rajagopalan

#include "H5Cpp.h"
#include <complex>

template<typename T> struct hdf5_datatype_traits {
	static H5::DataType const& dataType;
};

template<typename T>
class H5NativeComplex : public H5::CompType
{
public:
	H5NativeComplex() : CompType(sizeof(std::complex<T>)) {
		using namespace H5;
		DataType const& dataType = hdf5_datatype_traits<T>::dataType;
		insertMember(std::string("real"), 0, dataType);
		insertMember(std::string("imag"), dataType.getSize(), dataType);
		pack();
		lock();
	}
};

// complex data types
extern H5NativeComplex<float> NATIVE_COMPLEXFLOAT;
extern H5NativeComplex<double> NATIVE_COMPLEXDOUBLE;
extern H5NativeComplex<long double> NATIVE_COMPLEXLDOUBLE;


template<typename T>
void write_hdf5_scalar_attribute(H5::Group & group,
                                 std::string const& name,
                                 T const& value);

template<typename T>
void write_hdf5_scalar_attribute(H5::Group & group,
                                 std::string const& name,
                                 T const& value)
{
	using namespace H5;
	DataType const& dataType = hdf5_datatype_traits<T>::dataType;
	Attribute attr = group.createAttribute(name,
	                                       dataType,
	                                       DataSpace(H5S_SCALAR));
	attr.write(dataType, reinterpret_cast<void const *>(&value));

}

// explicit prototypes for polymorphic definitions
template<>
// extern
void write_hdf5_scalar_attribute(H5::Group & group,
                                 std::string const& name,
                                 std::string const& value);


template<typename T>
T read_hdf5_scalar_attribute(H5::Group & group,
                             std::string const& name)
{
	using namespace H5;
	DataType const& dataType = hdf5_datatype_traits<T>::dataType;
	Attribute attr = group.openAttribute(name);
	T value;
	attr.read(dataType, reinterpret_cast<void*>(&value));
	return value;
}

template<>
// extern
std::string read_hdf5_scalar_attribute(H5::Group & group,
                                       std::string const& name);




using namespace H5;
using namespace std;

//template<> DataType const& hdf5_datatype_traits<char>::dataType(PredType::NATIVE_CHAR);
//template<> DataType const& hdf5_datatype_traits<unsigned char>::dataType(PredType::NATIVE_UCHAR);
template<> DataType const& hdf5_datatype_traits<int>::dataType(PredType::NATIVE_INT);
//template<> DataType const& hdf5_datatype_traits<unsigned int>::dataType(PredType::NATIVE_UINT);
//template<> DataType const& hdf5_datatype_traits<float>::dataType(PredType::NATIVE_FLOAT);
//template<> DataType const& hdf5_datatype_traits<double>::dataType(PredType::NATIVE_DOUBLE);
//template<> DataType const& hdf5_datatype_traits<long double>::dataType(PredType::NATIVE_LDOUBLE);

// These decls must follow the above so that static initialization proceeds in order
//H5NativeComplex<float> NATIVE_COMPLEXFLOAT;
//H5NativeComplex<double> NATIVE_COMPLEXDOUBLE;
//H5NativeComplex<long double> NATIVE_COMPLEXLDOUBLE;

//template<> DataType const& hdf5_datatype_traits<complex<float> >::dataType(NATIVE_COMPLEXFLOAT);
//template<> DataType const& hdf5_datatype_traits<complex<double> >::dataType(NATIVE_COMPLEXDOUBLE);
//template<> DataType const& hdf5_datatype_traits<complex<long double> >::dataType(NATIVE_COMPLEXLDOUBLE);


template<>
void write_hdf5_scalar_attribute(H5::Group & group,
                                 std::string const& name,
                                 std::string const& value)
{
	StrType strType(0, value.length());
	Attribute attr = group.createAttribute(name,
	                                       strType,
	                                       DataSpace(H5S_SCALAR));
	attr.write(strType, reinterpret_cast<void const *>(value.c_str()));

}

template<>
string read_hdf5_scalar_attribute(Group & group,
                                  string const& name)
{
	Attribute attr = group.openAttribute(name);
	StrType strType = attr.getStrType();
	string value(strType.getSize()/sizeof(char)+1, '\0');
	attr.read(strType, reinterpret_cast<void*>(&value[0]));
	return value;
}



#endif // __HDF5_UTILS_H__
