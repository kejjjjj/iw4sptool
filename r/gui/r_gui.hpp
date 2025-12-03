#pragma once

#include "utils/vec.hpp"

class CBaseGui
{
public:
	virtual ~CBaseGui() = default;

	virtual void Toggle();

	virtual void Open();
	virtual void Close();
	virtual void OnClose() {};
	virtual void Render() = 0;
	constexpr virtual bool IsOpen() const noexcept { return b_open; }

	[[nodiscard]] fvec2 GetRealSize() const;
	[[nodiscard]] fvec2 GetRealPos() const;
	[[nodiscard]] fvec2 GetVirtualSize() const;
	[[nodiscard]] fvec2 GetVirtualPos() const;

protected:
	bool b_open = false;
};

