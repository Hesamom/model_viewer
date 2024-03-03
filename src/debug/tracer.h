
#ifndef TRACER_H
#define TRACER_H

namespace modelViewer::debug {
    class tracer {
    public:
        virtual ~tracer() = default;
        virtual void startCapture() = 0;
        virtual void endCapture() = 0;
		virtual void showUI() = 0;
    };
}
#endif //TRACER_H
