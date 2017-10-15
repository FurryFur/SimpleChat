//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : Input text into a buffer without stalling in a Win32 console window.
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#ifndef __INPUTLINEBUFFER_H__
#define __INPUTLINEBUFFER_H__

class CInputLineBuffer
{
public:
	CInputLineBuffer(unsigned int uBufferSize);
	~CInputLineBuffer();
	
	void ClearString();
	bool Update();
	void PrintToScreenTop();
	const char* GetString() const;

protected:
	unsigned int	m_uBufferSize;	//!< The total size of the buffer.
	unsigned int	m_uBufferPos;	//!< The position of the next char in the buffer to be entered by the user.
	char*			m_pBuffer;		//!< The buffer contents.
};

#endif