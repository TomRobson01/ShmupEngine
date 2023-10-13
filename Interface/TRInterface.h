#pragma once

#include "Interface/TRUIElement.h"

#include <vector>

class TRInterface
{
public:
	static TRInterface* const QInstance();

	void RenderUI();

	void AddUIElement(TRUIElement* apNewElement);

private:
	static TRInterface* instancePtr;

	std::vector<TRUIElement*> vUIStack;

};

