#include "RenderFunctions.h"
#include <iostream>

#include "ATHRenderer.h"

void RenderTest( ATHRenderer* pRenderer, ID3DXEffect* _pShader, ATHRenderNode* pNode )
{

	D3DXMATRIX matMVP = pRenderer->GetCamera()->GetViewMatrix() * pRenderer->GetCamera()->GetProjectionMatrix();

	D3DXMATRIX rot;
	D3DXMatrixRotationY( &rot, 0.1f );

	pNode->SetTransform( rot * pNode->GetTrasform() );

	_pShader->SetMatrix( "gWVP", &( pNode->GetTrasform() * matMVP ) );
	_pShader->SetTexture( "tex1", pNode->GetTexture().GetTexture() );

	_pShader->SetFloatArray( "multColor", float4( 1.0f, rand() % 1000 / 1000.0f, rand() % 1000 / 1000.0f, rand() % 1000 / 1000.0f ).Array, 4 );
	_pShader->CommitChanges();

	pRenderer->DrawMesh( pNode->GetMesh() );

}