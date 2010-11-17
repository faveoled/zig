#ifndef OPTIONSMENU_H
#define OPTIONSMENU_H

#include "menusupport.h"
#include "display.h"
#include <vector>

class OptionsMenu : public Menu
{
public:
	OptionsMenu();

	bool IsDone() const;
	virtual void Tick();
	virtual void Draw();
private:
	virtual void OnSelect( int id );
	virtual void OnLeft( int id );
	virtual void OnRight( int id );
	void LeftArrow( vec2 const& offset );
	void RightArrow( vec2 const& offset );

	bool m_Done;

	int	m_SelectedRes;
	bool m_Fullscreen;
	int m_BitDepth;
	enum ItemID { ID_RES, ID_FULLSCREEN, ID_DEPTH, ID_ACCEPT, ID_CANCEL };
};




inline bool OptionsMenu::IsDone() const
	{ return m_Done; }


#endif // PAUSEMENU_H

