#pragma once

namespace TerraX
{
#define MAKEINSTANCE(classname) \
public: \
	static classname& GetInstance() \
	{ \
		static classname T; \
		return T; \
	}
}