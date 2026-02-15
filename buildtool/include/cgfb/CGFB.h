#pragma once

#include <unordered_map>
#include <type_traits>
#include <fstream>
#include <string>


namespace cgfb
{

class AbstractStream
{
public:
	AbstractStream() = default;

	virtual inline int GetPosition() = 0;
	virtual inline void SetPosition(int position) = 0;
};


/**
 * @brief An abstract interface to write compatible C++ types into a CGFB formatted stream
 */
class CgfbWriter : public AbstractStream
{
public:
	/**
	 * @brief Writes integral types as signed 32-bit integers
	 */
	template <typename T>
	std::enable_if_t<std::is_integral_v<T>> Write(T value)
	{
		int32_t i32Value = value;
		WriteToStream((char*)&i32Value, sizeof(i32Value));
	}

	/**
	 * @brief Writes floating point types as signed 32-bit floats
	 */
	template <typename T>
	std::enable_if_t<std::is_floating_point_v<T>> Write(T value)
	{
		float f32Value = value;
		WriteToStream((char*)&f32Value, sizeof(f32Value));
	}

	/**
	 * @brief Writes null-terminated strings
	 */
	template <typename T>
	std::enable_if_t<std::is_same_v<T, const char *>> Write(T value)
	{
		int len = strlen(value);
		Write(len);

		WriteToStream((char*)value, len);
	}

	/**
	 * @brief Writes count bytes from data
	 */
	void Write(char* data, int count)
	{
		WriteToStream(data, count);
	}

	/**
	 * @brief Writes count bytes from data
	 */
	template<typename T>
	void Write(const std::vector<T>& vector)
	{
		Write(vector.size());

		WriteToStream((char*)vector.data(), vector.size() * sizeof(T));
	}

	/**
	 * @brief Writes a standard string
	 */
	template <typename T>
	std::enable_if_t<std::is_same_v<T, std::string>> Write(T value)
	{
		Write(value.size());

		WriteToStream(value.data(), value.size());
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

protected:
	/**
	 * @brief Writes some data into an arbitrary stream. Implementation behaviour is determined
	 * by derived classes (i.e. CgfbFileWriter)
	 */
	virtual void WriteToStream(char* data, int count) = 0;
};


/**
 * @brief An abstract interface to read compatible C++ types from a CGFB formatted stream 
 */
class CgfbReader : public AbstractStream
{
public:
	/**
	 * @brief Reads a 32-bit integer from the current CGFB file into an integral variable
	 */
	template <typename T>
	std::enable_if_t<std::is_integral_v<T>> Read(T *out)
	{
		int32_t i32;
		ReadFromStream((char*)&i32, sizeof(i32));

		*out = i32;
	}

	/**
	 * @brief Reads a 32-bit float from the current CGFB file into a floating point variable
	 */
	template <typename T>
	std::enable_if_t<std::is_floating_point_v<T>> Read(T* out)
	{
		float f32;
		ReadFromStream((char*)&f32, sizeof(f32));

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
		ReadFromStream(buff, len);
		*out = std::string(buff, len);
	}

	/**
	 * @brief Reads a string of bytes
	 */
	void Read(char* buff, int count)
	{
		ReadFromStream(buff, count);
	}

	/**
	 * @brief Reads an array of an arbitrary type and length directly into the memory of the supplied vector
	 */
	template<typename T>
	void Read(std::vector<T>* vector)
	{
		int size;
		Read(&size);

		vector->reserve(size);
		ReadFromStream((char*)vector->data(), size * sizeof(T));
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

protected:
	/**
	 * @brief Reads some data from an arbitrary stream. Implementation behaviour is determined
	 * by derived classes (i.e. CgfbFileReader)
	 */
	virtual void ReadFromStream(char* data, int count) = 0;
};


/**
 * @brief Formats and writes compatible values into a CGFB file
 */
class CgfbFileWriter : public CgfbWriter
{
public:
	CgfbFileWriter(const char* filePath);

	inline void SetPosition(int position) override
	{
		
	}

	inline int GetPosition() override
	{
		return m_File.tellp();
	}
	
protected:
	void WriteToStream(char* data, int count) override;

private:
	std::ofstream m_File;
};


/**
 * @brief Reads compatible values from a CGFB file
 */
class CgfbFileReader : public CgfbReader
{
public:
	CgfbFileReader(const char* filePath);
	
	void SeekStreamPosition(int position, std::ios_base::seekdir way = std::ios_base::beg);	
	
	inline void SetPosition(int position) override
	{
		
	}

	inline int GetPosition() override
	{
		return m_File.tellg();
	}

protected:
	void ReadFromStream(char *data, int count) override;

private:
	std::ifstream m_File;
};


/**
 * @brief Formats and writes compatible values into a block of memory containing CGFB data
 */
class CgfbMemoryWriter : public CgfbWriter
{
public:
	CgfbMemoryWriter() = default;

	inline const std::vector<char>& GetBuffer() const
	{
		return m_Buffer;
	}	
	
	inline void SetPosition(int position) override
	{
		
	}

	inline int GetPosition() override
	{
		return 0;
	}

protected:
	void WriteToStream(char *data, int count) override;

private:
	std::vector<char> m_Buffer;
};


/**
 * @brief Reads compatible values from a block of memory containing CGFB data
 */
class CgfbMemoryReader : public CgfbReader
{
public:
	CgfbMemoryReader(char* buffer);

	inline void SetPosition(int position) override
	{
	}

	inline int GetPosition() override
	{
		return 0;
	}

protected:
	void ReadFromStream(char *data, int count) override;

private:
	char* m_Buffer;
};

}