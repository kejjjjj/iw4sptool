#include "sl_string.hpp"
#include "utils/engine.hpp"

char* SL_ConvertToString(unsigned int stringValue){
	return Engine::Tools::Call<char*>(0x4E7440, stringValue);
}
