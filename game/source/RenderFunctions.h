#ifndef RENDERFUNCTIONS_H
#define RENDERFUNCTIONS_H


class ATHRenderNode;
class ATHRenderer;
struct ID3DXEffect;

void RenderTest(ATHRenderer* renderer, ID3DXEffect* _pShader, ATHRenderNode* node);

#endif