#pragma once

#include "column.h"
#include "dynamic_bitset.h"
#include <sstream>

using namespace terra;
namespace dataset
{
	class RowData
	{
		//DISABLE_COPY(Row);
		// using ValueChangeCB = std::function<void()>;
	private:
		Column& col_;
		char* data_buffer_{ nullptr };
		dynamic_bitset bitset_;

	public:
		RowData(Column& col);
		~RowData();

		// void RegValueChangeCB()
		std::string ToString();

		template <typename T>
		bool GetValue(const char* field_name, T& val);
		template <typename T>
		bool GetValue(int index, T& val);

		template <typename T>
		bool SetValue(const char* field_name, T val);
		template <typename T>
		bool SetValue(int index, T val);

		const char* GetValueString(const char* field_name);
		const char* GetValueString(int index);

		bool SetValueString(const char* field_name, const char* pVal);
		bool SetValueString(int index, const char* pVal);

	private:
		void AllocBuffer(uint32_t buffer_size);
	};


	template <typename T>
	bool RowData::GetValue(const char* field_name, T& val)
	{
		int nFieldIndex = col_.GetFieldIndex(field_name);
		return GetValue<T>(nFieldIndex, val);
	}
	template <typename T>
	bool RowData::GetValue(int index, T& val)
	{
		Field* field = col_.GetField(index);
		if (!field)
		{
			return false;
		}
		val = field->GetValue<T>(data_buffer_);
		return true;
	}

	template <typename T>
	bool RowData::SetValue(const char* field_name, T val)
	{
		int nFieldIndex = col_.GetFieldIndex(field_name);
		return SetValue<T>(nFieldIndex, val);
	}
	template <typename T>
	bool RowData::SetValue(int index, T val)
	{
		Field* field = col_.GetField(index);
		if (!field)
		{
			return false;
		}
		field->SetValue<T>(val, data_buffer_);
		bitset_.set(index);
		return true;
	}
}