#ifndef ATHRENDERFUNCTIONS_H
#define ATHRENDERFUNCTIONS_H

#include "ATHRenderer.h"

void DebugLineRender( ATHRenderer* pRenderer, ID3DXEffect* _pShader, ATHRenderNode* pNode )
{
	pNode->GetMesh()->RebuildBuffers();

	D3DXMATRIX matMVP = pRenderer->GetCamera()->GetViewMatrix() * pRenderer->GetCamera()->GetProjectionMatrix();

	pNode->SetTransform( pNode->GetTrasform() );
	_pShader->SetMatrix( "gWVP", &( pNode->GetTrasform() * matMVP ) );
	_pShader->CommitChanges();

	pRenderer->DrawMesh( pNode->GetMesh() );
}

#endif