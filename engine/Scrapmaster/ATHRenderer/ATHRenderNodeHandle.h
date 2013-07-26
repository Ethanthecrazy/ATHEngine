#ifndef ATHRENDERNODEHANDLE_H
#define ATHRENDERNODEHANDLE_H

class ATHRenderNode;

class ATHRenderNodeHandle
{
private:

	ATHRenderNode* m_pRenderNode;

	ATHRenderNodeHandle();

public:

	ATHRenderNodeHandle( ATHRenderNode* _pMyNode );

	friend class ATHRenderer;
};

#endif