CGFB (C++ game framework binary) files are binary files optimized for fast runtime streaming by CGF applications. The files are composed of arbitrarily long data blocks which are mapped out at the very beginning of the file. The manner in which different data types are written/read is described below, along with example file layouts and use cases.
## Primitive Integral Types
Types in C++ like *int*, *float*, *size_t*, etc, are simply written as an array of bytes with no regard for signing or endianness. This is a naive, temporary approach. Any type for which *std::is_integral_v\<T>* is true is written with the same method. The analogue for floating point types is *std::is_floating_point_v\<T>*
## Class/Struct Types
The read/write methods for class types must be implemented by the user, though, some common class types come already implemented.

The following class types are already r/w implemented:
- *std::unordered_map<KeyType, ValueType>*
- *std::vector\<ElemType>*
- *std::string*

## I/O Classes
Any stream which derives *cgfb::AbstractStream* can be used as a generic CGFB formatted stream. This allows for as much extensibility as is needed, but the current implementation comes with derivations which should be preferred and can suit most cases. 

The following is a list of the CGFB stream derivations:
- *cgfb::CgfbFileReader*
- *cgfb::CgfbFileWriter*
- *cgfb::CgfbMemoryReader*
- *cgfb::CgfbMemoryWriter*

It is important to note that the CgfbFile implementations enforce the block layout described at the top of this document. 

