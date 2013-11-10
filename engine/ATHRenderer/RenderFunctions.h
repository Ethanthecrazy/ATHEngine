#ifndef RENDERFUNCTIONS_H
#define RENDERFUNCTIONS_H

#include "ATHRenderNode.h"

class ATHRenderer;

void RenderTest(ATHRenderer* renderer, ID3DXEffect* _pShader, ATHRenderNode* node);

void DebugLineRender( ATHRenderer* renderer, ID3DXEffect* _pShader, ATHRenderNode* node );


#endif