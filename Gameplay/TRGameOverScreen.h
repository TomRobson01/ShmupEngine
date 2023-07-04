#pragma once
class TRGameOverScreen
{
public:
	static TRGameOverScreen* const QInstance();

	void Update();

private:
	TRGameOverScreen();
	static TRGameOverScreen* instancePtr;
};

