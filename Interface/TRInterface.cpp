#include "TRInterface.h"
#include "Rendering/TRRenderer.h"

TRInterface* TRInterface::instancePtr;
TRInterface* const TRInterface::QInstance()
{
	if (instancePtr == nullptr)
	{
		instancePtr = new TRInterface();
	}
	return instancePtr;
}

void TRInterface::RenderUI()
{
	for (TRUIElement* pElement : vUIStack)
	{
		if (pElement->QVisible())
		{
			TRRenderer::QInstance().AddRenderTarget(
				pElement->QScreenX(),
				pElement->QScreenY(),
				0,						// UI does not support Z depth
				0,						// UI does not support rotation
				pElement->QTexture(),
				pElement->QScale());
		}
	}
}

void TRInterface::AddUIElement(TRUIElement* apNewElement)
{
	float fXPos = apNewElement->QXPos();
	float fYPos = apNewElement->QYPos();

	TRRenderer::QInstance().ScreenPositionToRenderPosition(fXPos, fYPos, fXPos, fYPos);
	apNewElement->SetRenderPos(fXPos, fYPos);

	vUIStack.push_back(apNewElement);
}
