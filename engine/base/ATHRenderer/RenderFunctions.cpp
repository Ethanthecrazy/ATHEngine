#include "RenderFunctions.h"
#include <iostream>

#include "ATHRenderer.h"

void RenderTest( ATHRenderer* pRenderer, ID3DXEffect* _pShader, ATHRenderNode* pNode )
{

	D3DXMATRIX matMVP = pRenderer->GetCamera()->GetViewMatrix() * pRenderer->GetCamera()->GetProjectionMatrix();

	_pShader->SetMatrix( "gWVP", &( pNode->GetTrasform() * matMVP ) );
	_pShader->SetTexture( "tex1", pNode->GetTexture().GetTexture() );

	_pShader->SetFloatArray( "multColor", float4( 1.0f, 1.0f, 0.5f, 0.5f ).Array, 4 );
	_pShader->CommitChanges();

	pRenderer->GetDevice()->SetStreamSource( 0, pNode->GetMesh()->GetVertexBuffer(), 0, sizeof( sVertPosNormUV ) );
	pRenderer->GetDevice()->SetIndices( pNode->GetMesh()->GetIndexBuffer() );
	pRenderer->GetDevice()->DrawIndexedPrimitive( pNode->GetMesh()->GetPrimativeType(), 0, 0, pNode->GetMesh()->GetVertexCount(), 0, 2 );

}