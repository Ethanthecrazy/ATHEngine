#include "RenderFunctions.h"
#include <iostream>

#include "ATHRenderer.h"

void RenderTest( ATHRenderer* pRenderer, ID3DXEffect* _pShader, ATHRenderNode* pNode )
{

	D3DXMATRIX matMVP = pRenderer->GetCamera()->GetViewMatrix() * pRenderer->GetCamera()->GetProjectionMatrix();

	D3DXMATRIX mat = pNode->GetTrasform() * matMVP;

	_pShader->SetMatrix( "gWVP", &mat );
	_pShader->SetTexture( "tex1", pNode->GetTexture().GetTexture() );

	_pShader->SetFloatArray( "multColor", float4( 1.0f ).Array, 4 );
	_pShader->CommitChanges();

	pRenderer->DrawMesh( pNode->GetMesh() );

}
//================================================================================
void DebugLineRender( ATHRenderer* pRenderer, ID3DXEffect* _pShader, ATHRenderNode* pNode )
{
	pNode->GetMesh()->RebuildBuffers();

	D3DXMATRIX matMVP = pRenderer->GetCamera()->GetViewMatrix() * pRenderer->GetCamera()->GetProjectionMatrix();

	pNode->SetTransform( pNode->GetTrasform() );
	_pShader->SetMatrix( "gWVP", &( pNode->GetTrasform() * matMVP ) );
	_pShader->CommitChanges();

	pRenderer->DrawMesh( pNode->GetMesh() );
}
//================================================================================