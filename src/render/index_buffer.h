﻿
#ifndef MODEL_VIEWER_INDEXBUFFER_H
#define MODEL_VIEWER_INDEXBUFFER_H

namespace modelViewer::render {
        class index_buffer
        {
        private:
            unsigned int m_BufferId;
            int m_Count;
        public:
            explicit index_buffer(std::vector<unsigned int>& data);
            ~index_buffer();
            void bind();
            void setName(const std::string& name);
            int getCount();
            void draw();
        };

    }

#endif //MODEL_VIEWER_INDEXBUFFER_H