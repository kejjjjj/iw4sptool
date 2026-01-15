#include "sys_time.hpp"
#include "utils/engine.hpp"

unsigned long Sys_Milliseconds() {
	return Engine::Tools::Call<unsigned long>(0x44E1B0);
}