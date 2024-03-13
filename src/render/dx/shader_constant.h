﻿
#ifndef SHADER_CONSTANT_H
#define SHADER_CONSTANT_H

namespace modelViewer::render::dx
{
	struct constant_variable
	{
		int size = 0;
		int offset = 0;
		std::string name;

		bool isEqual(constant_variable& other)
		{
			return size == other.size && offset == other.offset && name == other.name;
		}
	};

	struct constant_block
	{
		int size = 0;
		UINT bindPoint = 0;
		std::wstring name;
		std::vector<constant_variable> variables;

		bool isEqual(constant_block& block)
		{
			if (block.size != size || block.variables.size() != variables.size() || block.name != name)
			{
				return false;
			}

			auto count = block.variables.size();
			for (int i = 0; i < count; ++i)
			{
				if (!block.variables[i].isEqual(variables[i]))
				{
					return false;
				}
			}

			return true;
		}
	};
}
#endif //SHADER_CONSTANT_H
