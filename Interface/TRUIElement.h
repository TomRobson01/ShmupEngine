#pragma once

// UI Elements are responsible for maintaining their own texture, as well as their position. UI positions are relative to the screen, not the world.
// Rendering them is handled from TRInterface. UI Elements need only adding to the UI stack once, and must explicitly be removed.

class TRUIElement
{
public:
	TRUIElement(float afXPos, float afYPos, float afScale, unsigned int auiTexture)
	{
		fXPos = afXPos;
		fYPos = afYPos;
		fScale = afScale;
		uiTexture = auiTexture;
	}

	void SetVisibility(bool abNewVisibility) { bVisible = abNewVisibility; }

	void SetRenderPos(const float afScreenX, const float afScreenY) 
	{ 
		fScreenX = afScreenX;
		fScreenY = afScreenY;
	}

	const float QXPos() const			{ return fXPos; }
	const float QYPos() const			{ return fYPos; }
	const float QScreenX() const		{ return fScreenX; }
	const float QScreenY() const		{ return fScreenY; }
	const float QScale() const			{ return fScale; }
	const unsigned int QTexture() const { return uiTexture; }
	const bool QVisible() const			{ return bVisible; }

private:
	float fXPos = 0.0f;
	float fYPos = 0.0f;
	float fScale = 1.0f;

	float fScreenX = 0.0f;
	float fScreenY = 0.0f;

	unsigned int uiTexture;

	bool bVisible = true;

};

