#pragma once
class TRMainMenu
{
public:
	static TRMainMenu* const QInstance();

	void Update();

private:
	TRMainMenu();
	static TRMainMenu* instancePtr;
};

