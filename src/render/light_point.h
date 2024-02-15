
#ifndef LIGHT_POINT_H
#define LIGHT_POINT_H

#include "glm/vec3.hpp"

namespace modelViewer::render
{
	class object_renderer;

	class light_point
	{
	private:
		float m_Range;
		float m_InsideRange;
	public:
		glm::vec3 position;
		glm::vec3 ambient;
		glm::vec3 diffuse;
		int id = 0;
		std::shared_ptr<object_renderer> object;
		
		void setRange(float range, float insideRange)
		{
			if (range < 0 || insideRange < 0)
			{
				throw std::runtime_error("ranges can not be less than zero!");
			}

			if (range < insideRange)
			{
				throw std::runtime_error("inside range can not be less than range!");
			}
			
			m_Range = range;
			m_InsideRange = insideRange;
		}
		
		void getRanges(float& range, float& insideRange)
		{
			range = m_Range;
			insideRange = m_InsideRange;
		}
	};
}

#endif //LIGHT_POINT_H
