#pragma once

#include <unordered_map>
#include <type_traits>
#include <fstream>
#include <string>


/**
 * @brief An interface to write common C++ types into a CGFB formatted file 
 */
class CgfbWriter
{
public:
	CgfbWriter(const char* filePath);
	CgfbWriter(const CgfbWriter& other) = delete;
	~CgfbWriter();

	/**
	 * @brief Writes integral types as signed 32-bit integers
	 */
	template <typename T>
	std::enable_if_t<std::is_integral_v<T>> Write(T value)
	{
		int32_t i32Value = value;
		m_File.write((char *)&i32Value, sizeof(i32Value));
	}

	/**
	 * @brief Writes floating point types as signed 32-bit floats
	 */
	template <typename T>
	std::enable_if_t<std::is_floating_point_v<T>> Write(T value)
	{
		float f32Value = value;
		m_File.write((char *)&f32Value, sizeof(f32Value));
	}

	/**
	 * @brief Writes null-terminated strings
	 */
	template <typename T>
	std::enable_if_t<std::is_same_v<T, const char *>> Write(T value)
	{
		int len = strlen(value);

		Write(len);

		m_File.write((char *)value, len);
	}

	/**
	 * @brief Writes count bytes from data
	 */
	void Write(char* data, int count)
	{
		m_File.write(data, count);
	}

	/**
	 * @brief Writes a standard string
	 */
	template <typename T>
	std::enable_if_t<std::is_same_v<T, std::string>> Write(T value)
	{
		Write(value.size());

		m_File.write(value.data(), value.size());
	}

	/**
	 * @brief Writes unordered maps if the map's key and value types are writable
	 */
	template <typename KT, typename VT>
	void Write(std::unordered_map<KT, VT> value)
	{
		Write((int)value.size());

		for (const auto &[k, v] : value)
		{
			Write(k);
			Write(v);
		}
	}

private:
	std::ofstream m_File;
};


/**
 * @brief An interface to read common C++ types from a CGFB formatted file 
 */
class CgfbReader
{
public:
	CgfbReader(const char *filePath);
	CgfbReader(const CgfbWriter &other) = delete;
	~CgfbReader();

	/**
	 * @brief Reads a 32-bit integer from the current CGFB file into an integral variable
	 */
	template <typename T>
	std::enable_if_t<std::is_integral_v<T>> Read(T *out)
	{
		int32_t i32;
		m_File.read((char*)&i32, sizeof(i32));

		*out = i32;
	}

	/**
	 * @brief Reads a 32-bit float from the current CGFB file into a floating point variable
	 */
	template <typename T>
	std::enable_if_t<std::is_floating_point_v<T>> Read(T* out)
	{
		float f32;
		m_File.read((char*)&f32, sizeof(f32));

		*out = f32;
	}

	/**
	 * @brief Reads a standard string from the current CGFB file
	 */
	template <typename T>
	std::enable_if_t<std::is_same_v<T, std::string>> Read(std::string* out)
	{
		int len;
		Read(&len);

		char* buff = new char[len];
		m_File.read(buff, len);
		*out = std::string(buff, len);
	}

	/**
	 * @brief Reads a standard unordered map from the current CGFB file
	 */
	template <typename KT, typename VT>
	void Read(std::unordered_map<KT, VT> *out)
	{
		int numElements;
		Read(&numElements);
		out->reserve(numElements);

		KT currentKey;
		VT currentValue;

		for(int i = 0; i < numElements; i++)
		{
			Read<KT>(&currentKey);
			Read<VT>(&currentValue);

			(*out)[currentKey] = currentValue;
		}
	}

private:
	std::ifstream m_File;
};