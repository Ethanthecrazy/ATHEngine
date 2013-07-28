#include "RenderFunctions.h"
#include <iostream>

#include "ATHRenderer.h"

void RenderTest( ATHRenderer* pRenderer, ID3DXEffect* _pShader, ATHRenderNode* pNode )
{

	HRESULT hResult;

	D3DXMATRIX matMVP = pRenderer->GetCamera()->GetViewMatrix() * pRenderer->GetCamera()->GetProjectionMatrix();

	hResult = _pShader->SetMatrix( "gWVP", &( pNode->GetTrasform() * matMVP ) );
	hResult = _pShader->SetTexture( "tex1", pNode->GetTexture().GetTexture() );

	hResult = _pShader->SetFloatArray( "multColor", float4( 1.0f, 1.0f, 0.5f, 0.5f ).Array, 4 );
	hResult = _pShader->CommitChanges();

	hResult = pRenderer->GetDevice()->SetStreamSource( 0, pNode->GetMesh()->GetVertexBuffer(), 0, sizeof( sVertPosNormUV ) );
	hResult = pRenderer->GetDevice()->SetIndices( pNode->GetMesh()->GetIndexBuffer() );
	hResult = pRenderer->GetDevice()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, pNode->GetMesh()->GetVertexCount(), 0, 2 );

}