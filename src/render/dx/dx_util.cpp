
#include "dx_util.h"
#include "../../common/string_util.h"

void modelViewer::render::dx::attempt(HRESULT result)
{
	if(FAILED(result))
	{
		_com_error err(result);
		std::wstring msg = err.ErrorMessage();
		std::cerr << convertWideToANSI(msg) << std::endl;
		throw std::runtime_error("Operation failed");
	}
}