
#ifndef NOT_IMPLEMENTED_ERROR_H
#define NOT_IMPLEMENTED_ERROR_H

namespace modelViewer::error
{
	class not_implemented_error : public std::logic_error {
	public:
		virtual char const* what() const
		{ return "Function not yet implemented."; }
	};
}

#endif //NOT_IMPLEMENTED_ERROR_H
